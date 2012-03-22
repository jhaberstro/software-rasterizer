#include "DefaultRasteriser.hpp"
#include <tuple>
#include <array>
#include <cmath>
#include <glm/gtc/swizzle.hpp>

// inline float TriangleArea(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
// 	return 0.5f * glm::length(glm::cross(v1 - v0, v2 - v0));
// }

// template<typename TPointsContainer >
// inline void DrawPoints(Framebuffer& fb, TPointsContainer const& v, uint32_t color) {
// 	for (glm::vec2 p0 : v) {
// 		fb.set_pixel(p0.x,      p0.y, reinterpret_cast< uint8_t* >(&color));
// 		fb.set_pixel(p0.x+1.0f, p0.y, reinterpret_cast< uint8_t* >(&color));
// 		fb.set_pixel(p0.x-1.0f, p0.y, reinterpret_cast< uint8_t* >(&color));
// 		fb.set_pixel(p0.x,      p0.y+1.0f, reinterpret_cast< uint8_t* >(&color));
// 		fb.set_pixel(p0.x,      p0.y+1.0f, reinterpret_cast< uint8_t* >(&color));
// 	}
// }

// inline void DrawFlatBottomTriangle(StateContext& ctx, Shader& fsh, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, TriangleData& triangle) {
// 	glm::vec3& v0 = get_triangle_vert0(triangle);
// 	glm::vec3& v1 = get_triangle_vert1(triangle);
// 	glm::vec3& v2 = get_triangle_vert2(triangle);
// 	VaryingData& varying0 = get_triangle_varying0(triangle);
// 	VaryingData& varying1 = get_triangle_varying1(triangle);
// 	VaryingData& varying2 = get_triangle_varying2(triangle);
// 	float area = TriangleArea(v0, v1, v2);
// 	float invz0 = 1.0f / v0.z;
// 	float invz1 = 1.0f / v1.z;
// 	float invz2 = 1.0f / v2.z;

// 	float dy = p0.y < p1.y ? 1 : -1;
// 	float dx_left  = (p1.x - p0.x) / std::fabsf(p1.y - p0.y);
// 	float dx_right = (p2.x - p0.x) / std::fabsf(p2.y - p0.y);
// 	float dz_left  = (p1.z - p0.z) / std::fabsf(p1.y - p0.y);
// 	float dz_right = (p2.z - p0.z) / std::fabsf(p2.y - p0.y);
// 	float x_left = p0.x, x_right = p0.x, z_left = p0.z, z_right = p0.z;
// 	for (float y = p0.y; dy == -1 ? y >= p1.y : y <= p1.y; y += dy) {
// 		for (float x = x_left; x <= x_right; x += 1.0f) {
// 			float depth = z_left + ((z_right - z_left) * ((x - x_left) / (x_right - x_left)));
// 			float currentDepth;
// 			ctx.depth_buffer().get_pixel(x, y, reinterpret_cast< uint8_t* >(&currentDepth));
// 			if (depth > currentDepth) {
// 				continue;
// 			}

// 			ctx.depth_buffer().set_pixel(x, y, reinterpret_cast< uint8_t* >(&depth));
			
// 			glm::vec4 color = fsh.ffunc(std::get< 3 >(triangle), fsh.uniforms);
// 			ctx.framebuffer().set_pixel(ceil(x), ceil(y), reinterpret_cast< uint8_t* >(&color));
// 		}

// 		x_left += dx_left;
// 		x_right += dx_right;
// 		z_left += dz_left;
// 		z_right += dz_right;
// 	}
// }


void default_rasteriser(StateContext& ctx, Shader& fsh, TriangleData& triangle) {
	glm::vec3& p0 = get_triangle_vert0(triangle);
	glm::vec3& p1 = get_triangle_vert0(triangle);
	glm::vec3& p2 = get_triangle_vert0(triangle);
    float minx = std::min(std::min(p0.x, p1.x), p2.x);
    float miny = std::min(std::min(p0.y, p1.y), p2.y);
    float maxx = std::max(std::max(p0.x, p1.x), p2.x);
    float maxy = std::max(std::max(p0.y, p1.y), p2.y);

    // All derived from:
    //  float edge01 = (dx01 * (y - p0.y)) - (dy01 * (x - p0.x));
    //  float edge12 = (dx12 * (y - p1.y)) - (dy12 * (x - p1.x));
    //  float edge20 = (dx20 * (y - p2.y)) - (dy20 * (x - p2.x));
    float dx01 = (p1.x - p0.x);
    float dx12 = (p2.x - p1.x);
    float dx20 = (p0.x - p2.x);
    float dy01 = (p1.y - p0.y);
    float dy12 = (p2.y - p1.y);
    float dy20 = (p0.y - p2.y);
    float c01 = (dx01 * -p0.y) + (dy01 * p0.x);
    float c12 = (dx12 * -p1.y) + (dy12 * p1.x);
    float c20 = (dx20 * -p2.y) + (dy20 * p2.x);
    float cy01 = (dx01 * miny) + (dy01 * -minx) + c01;
    float cy12 = (dx12 * miny) + (dy12 * -minx) + c12;
    float cy20 = (dx20 * miny) + (dy20 * -minx) + c20;

    // Ax + By + Cz + D = 0, where (A, B, C) is normal to the tri-plane
    // -> z = -A/C*x - B/C*y - D
    glm::vec3 triNormal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
    float dzdx = -triNormal[0] / triNormal[2];
    float dzdy = -triNormal[1] / triNormal[2];
    float cz = p0.z + (dzdx * (minx - p0.x)) + (dzdy * (miny - p0.y));

    // Correct for fill convention
    if (dy01 < 0 || (dy01 == 0.0f && dx01 > 0)) c01 += 1;
    if (dy12 < 0 || (dy12 == 0.0f && dx12 > 0)) c12 += 1;
    if (dy20 < 0 || (dy20 == 0.0f && dx20 > 0)) c20 += 1;

    uint32_t color = 0xff0000ff;
    for (float y = miny; y < maxy; y += 1.0f) {
        float cx01 = cy01;
        float cx12 = cy12;
        float cx20 = cy20;
        float z    = cz;
        for (float x = minx; x < maxx; x += 1.0f) {
            if (cx01 >= 0.0f && cx12 >= 0.0f && cx20 >= 0.0f) {
                float currentDepth;
                ctx.depth_buffer().get_pixel(x, y, reinterpret_cast< uint8_t* >(&currentDepth));
                if (z > currentDepth) {
                    continue;
                }

                ctx.depth_buffer().set_pixel(x, y, reinterpret_cast< uint8_t* >(&z));
                ctx.framebuffer().set_pixel(x, y, reinterpret_cast< uint8_t* >(&color));
            }

            cx01 -= dy01;
            cx12 -= dy12;
            cx20 -= dy20;
            z    += dzdx;
        }

        cy01 += dx01;
        cy12 += dx12;
        cy20 += dx20;
        cz   += dzdy;
    }
}
