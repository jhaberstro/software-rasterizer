#include "Renderer.hpp"
#include <tuple>
#include <cstdio>
#include <cassert>

inline void process_vert(Renderer& renderer, glm::vec4& processedVert, VaryingData& vshOutput) {
	assert(vshOutput[0].size == 4);
	glm::vec4& vert = vshOutput[0].v4;

	// calculate normalized device coordinates
	float invW = 1.0f / vert[3];
	processedVert[0] = vert[0] * invW;
	processedVert[1] = vert[1] * invW;
	processedVert[2] = vert[2] * invW;
	processedVert[3] = invW;

	// convert to window coordinates
	float w = renderer.viewport().w, h = renderer.viewport().h;
	float far = renderer.viewport().far, near = renderer.viewport().near;
	processedVert.x = (0.5f * w * processedVert.x) + float(renderer.viewport().x) + (0.5f * w);
	processedVert.y = (0.5f * h * processedVert.y) + float(renderer.viewport().y) + (0.5f * h);
	processedVert.z = ((0.5f * (far - near)) * processedVert.z) + (0.5f * (far + near));
};

void Renderer::draw(const std::vector< VertexArray > const& attributes, size_t start, size_t num) {
	assert(_currentVsh != nullptr);
	assert(_currentFsh != nullptr);

	size_t increment;
	size_t startOffset;
	size_t indices1[] = {-2, -1, 0};
	size_t indices2[] = {-2, -1, 0};
	if (_primitiveTopology == PrimitiveTopology::TriangleList) {
		increment = 3;
		startOffset = 2;
	}
	else if (_primitiveTopology == PrimitiveTopology::TriangleStrip) {
		increment = 1;
		startOffset = 2;
		indices2[0] = -1, indices2[1] = -2, indices2[2] = 0;
	}
	else {
		assert(false);
	}

	if (_winding == PolygonWinding::Clockwise) {
		std::swap(indices1[1], indices1[2]);
		std::swap(indices2[1], indices2[2]);
	}

	for (size_t i = start + startOffset; i < (start + num); i += increment) {
		TriangleData triangle = std::make_tuple(
			glm::vec4(), glm::vec4(), glm::vec4(),
			_currentVsh->vfunc(i + indices1[0], attributes, _currentVsh->uniforms),
			_currentVsh->vfunc(i + indices1[1], attributes, _currentVsh->uniforms),
			_currentVsh->vfunc(i + indices1[2], attributes, _currentVsh->uniforms)
		);

		process_vert(*this, get_triangle_vert0(triangle), get_triangle_varying0(triangle));
		process_vert(*this, get_triangle_vert1(triangle), get_triangle_varying1(triangle));
		process_vert(*this, get_triangle_vert2(triangle), get_triangle_varying2(triangle));
		default_rasteriser(this, *_currentFsh, triangle);

		std::swap(indices1, indices2);
	}
}

void Renderer::set_framebuffer(size_t w, size_t h, size_t bytesPerPixel) {
	if (_framebuffer != nullptr) {
		delete _framebuffer;
	}

	if (_depthBuffer != nullptr) {
		delete _depthBuffer;
	}

	_framebuffer = new Framebuffer(w, h, bytesPerPixel);
	_depthBuffer = new Framebuffer(w, h, 4);
}