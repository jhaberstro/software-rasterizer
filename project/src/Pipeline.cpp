#include "Pipeline.hpp"
#include <tuple>
#include <cstdio>

inline void process_vert(StateContext& ctx, glm::vec3& processedVert, VaryingData& vshOutput) {
	assert(vshOutput[0].type == Vec4);
	glm::vec4& vert = vshOutput[0].v4;

	// calculate normalized device coordinates
	float invW = 1.0f / vert[3];
	processedVert[0] = vert[0] * invW;
	processedVert[1] = vert[1] * invW;
	processedVert[2] = vert[2] * invW;

	// convert to window coordinates
	float w = ctx.viewport().w, h = ctx.viewport().h;
	float far = ctx.viewport().far, near = ctx.viewport().near;
	processedVert.x = (0.5f * w * processedVert.x) + float(ctx.viewport().x) + (0.5f * w);
	processedVert.y = (0.5f * h * processedVert.y) + float(ctx.viewport().y) + (0.5f * h);
	processedVert.z = ((0.5f * (far - near)) * processedVert.z) + (0.5f * (far + near));
};

void Pipeline::execute(RasteriserFunc rasterf) {
	while (_context->drawCalls.empty() == false) {
		DrawCall* drawCall = _context->drawCalls.front();
		_context->drawCalls.pop();

		// Assuming straight up triangles, not triangle lists or quads or lines or whatever
		// Thus the increment by 3 on i.
		// TODO - support triangle lists
		for (size_t i = drawCall->startVert; i < drawCall->numVerts; i += 3) {
			TriangleData triangle = std::make_tuple(
				glm::vec3(), glm::vec3(), glm::vec3(),
				drawCall->vertexShader->vfunc(i,   *drawCall->attributeData, drawCall->vertexShader->uniforms),
				drawCall->vertexShader->vfunc(i+1, *drawCall->attributeData, drawCall->vertexShader->uniforms),
				drawCall->vertexShader->vfunc(i+2, *drawCall->attributeData, drawCall->vertexShader->uniforms)
			);

			process_vert(*_context, get_triangle_vert0(triangle), get_triangle_varying0(triangle));
			process_vert(*_context, get_triangle_vert1(triangle), get_triangle_varying1(triangle));
			process_vert(*_context, get_triangle_vert2(triangle), get_triangle_varying2(triangle));
			rasterf(*_context, *drawCall->fragmentShader, triangle);
		}

		delete drawCall;
	}
}