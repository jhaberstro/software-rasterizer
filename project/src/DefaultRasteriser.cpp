#include "DefaultRasteriser.hpp"
#include "Renderer.hpp"
#include <tuple>
#include <array>
#include <cmath>
#include <glm/gtc/swizzle.hpp>
#include "FixedPointMath.hpp"

// void default_rasteriser(Renderer* renderer, Shader const& fsh, TriangleData& triangle) {
// 	glm::vec3& p0 = get_triangle_vert0(triangle);
// 	glm::vec3& p1 = get_triangle_vert1(triangle);
// 	glm::vec3& p2 = get_triangle_vert2(triangle);
//     float minx = std::max(0.0f, std::min(std::min(p0.x, p1.x), p2.x));
//     float miny = std::max(0.0f, std::min(std::min(p0.y, p1.y), p2.y));
//     float maxx = std::min(float(renderer->framebuffer().width() - 1),  std::max(std::max(p0.x, p1.x), p2.x));
//     float maxy = std::min(float(renderer->framebuffer().height() - 1), std::max(std::max(p0.y, p1.y), p2.y));

//     printf("min: (%f, %f)\tmax: (%f, %f)\n", float(minx), float(miny), float(maxx), float(maxy));

//     // All derived from:
//     //  float edge01 = (dx01 * (y - p0.y)) - (dy01 * (x - p0.x));
//     //  float edge12 = (dx12 * (y - p1.y)) - (dy12 * (x - p1.x));
//     //  float edge20 = (dx20 * (y - p2.y)) - (dy20 * (x - p2.x));
//     float dx01 = (p1.x - p0.x);
//     float dx12 = (p2.x - p1.x);
//     float dx20 = (p0.x - p2.x);
//     float dy01 = (p1.y - p0.y);
//     float dy12 = (p2.y - p1.y);
//     float dy20 = (p0.y - p2.y);
//     float c01 = (dx01 * -p0.y) + (dy01 * p0.x);
//     float c12 = (dx12 * -p1.y) + (dy12 * p1.x);
//     float c20 = (dx20 * -p2.y) + (dy20 * p2.x);

//     // Correct for fill convention
//     if (dy01 < 0 || (dy01 == 0.0f && dx01 > 0)) c01 += 1;
//     if (dy12 < 0 || (dy12 == 0.0f && dx12 > 0)) c12 += 1;
//     if (dy20 < 0 || (dy20 == 0.0f && dx20 > 0)) c20 += 1;

//     float cy01 = (dx01 * miny) + (dy01 * -minx) + c01;
//     float cy12 = (dx12 * miny) + (dy12 * -minx) + c12;
//     float cy20 = (dx20 * miny) + (dy20 * -minx) + c20;

//     // Ax + By + Cz + D = 0, where (A, B, C) is normal to the tri-plane
//     // -> z = -A/C*x - B/C*y - D
//     glm::vec3 triNormal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
//     float dzdx = -triNormal[0] / triNormal[2];
//     float dzdy = -triNormal[1] / triNormal[2];
//     float cz = p0.z + (dzdx * (minx - p0.x)) + (dzdy * (miny - p0.y));

//     auto& varying0 = get_triangle_varying0(triangle);
//     auto& varying1 = get_triangle_varying1(triangle);
//     auto& varying2 = get_triangle_varying2(triangle);
//     VaryingData interpolatedVaryings;
//     VaryingData xgradients;
//     VaryingData ygradients;
//     interpolatedVaryings.reserve(varying0.size() - 1);
//     xgradients.reserve(varying0.size() - 1);
//     ygradients.reserve(varying0.size() - 1);
//     for (int i = 1; i < varying0.size(); ++i) {
//         ShaderVariable& sv0 = varying0[i];
//         ShaderVariable& sv1 = varying1[i];
//         ShaderVariable& sv2 = varying2[i];
//         switch(sv0.type) {
//             case Vec4: {
//                 glm::vec4 A    = (sv2.v4 - sv0.v4) * (p1.y - p0.y) - (sv1.v4 - sv0.v4) * (p2.y - p0.y);                
//                 glm::vec4 B    = (p2.x - p0.x) * (sv1.v4 - sv0.v4) - (p1.x - p0.x) * (sv2.v4 - sv0.v4);
//                 float invC     = 1.0f / ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y)); 
//                 glm::vec4 dvdx = -A * invC;
//                 glm::vec4 dvdy = -B * invC;
//                 glm::vec4 cv   = sv0.v4 + (dvdx * (minx - p0.x)) + (dvdy * (miny - p0.y));
//                 interpolatedVaryings.push_back(cv);
//                 xgradients.push_back(dvdx);
//                 ygradients.push_back(dvdy);
//                 break;
//             }

//             default: {
//                 assert(false);
//                 break;
//             }
//         }
//     }

//     size_t bufferSize = interpolatedVaryings.size() * sizeof(ShaderVariable);
//     uint8_t varyingsBuffer[bufferSize];
//     ShaderVariable* varyings = reinterpret_cast< ShaderVariable* >(&varyingsBuffer[0]);
//     for (float y = miny; y <= maxy; y += 1.0f) {
//         float cx01 = cy01;
//         float cx12 = cy12;
//         float cx20 = cy20;
//         float z    = cz;
//         std::memcpy(varyings, &interpolatedVaryings[0], bufferSize);
//         for (float x = minx; x <= maxx; x += 1.0f) {
//             if (cx01 > 0.0f && cx12 > 0.0f && cx20 > 0.0f) {
//                 float currentDepth;
//                 renderer->depth_buffer().get_pixel(x, y, &currentDepth);
//                 if (z <= currentDepth) {
//                     glm::vec4 color = fsh.ffunc(varyings, fsh.uniforms);
//                     color *= glm::vec4(255.0f);
//                     uint32_t pixel = (static_cast< uint32_t >(color[3]) << 24) | (static_cast< uint32_t >(color[2]) << 16) | (static_cast< uint32_t >(color[1]) << 8) | static_cast< uint32_t >(color[0]);  
//                     renderer->depth_buffer().set_pixel(x, y, &z);
//                     renderer->framebuffer().set_pixel(x, y, &pixel);
//                 }
//             }

//             cx01 -= dy01;
//             cx12 -= dy12;
//             cx20 -= dy20;
//             z    += dzdx;
//             for (int i = 0; i < interpolatedVaryings.size(); ++i) {
//                 varyings[i] += xgradients[i];
//             }
//         }

//         cy01 += dx01;
//         cy12 += dx12;
//         cy20 += dx20;
//         cz   += dzdy;
//         for (int i = 0; i < interpolatedVaryings.size(); ++i) {
//             interpolatedVaryings[i] += ygradients[i];
//         }
//     }
// }

void default_rasteriser(Renderer* renderer, Shader const& fsh, TriangleData& triangle) {
    typedef FixedPointVector3< 4 > vec3_fp;
    typedef FixedPointReal< 4 > float_fp;

    glm::vec3& rp0 = get_triangle_vert0(triangle);
    glm::vec3& rp1 = get_triangle_vert1(triangle);
    glm::vec3& rp2 = get_triangle_vert2(triangle);
    glm::ivec3 p0(rp0.x, rp0.y, rp0.z);
    glm::ivec3 p1(rp1.x, rp1.y, rp1.z);
    glm::ivec3 p2(rp2.x, rp2.y, rp2.z);
    int minx = (std::max(0.0f, std::min(std::min(rp0.x, rp1.x), rp2.x)));
    int miny = (std::max(0.0f, std::min(std::min(rp0.y, rp1.y), rp2.y)));
    int maxx = (std::min(float(renderer->framebuffer().width() - 1),  std::max(std::max(rp0.x, rp1.x), rp2.x)));
    int maxy = (std::min(float(renderer->framebuffer().height() - 1), std::max(std::max(rp0.y, rp1.y), rp2.y)));
    p0 = rp0 * 16.0f;
    p1 = rp1 * 16.0f;
    p2 = rp2 * 16.0f;

    int dx01 = (p1.x - p0.x);
    int dx12 = (p2.x - p1.x);
    int dx20 = (p0.x - p2.x);
    int dy01 = (p1.y - p0.y);
    int dy12 = (p2.y - p1.y);
    int dy20 = (p0.y - p2.y);
    int fdx01 = dx01 << 4;
    int fdx12 = dx12 << 4;
    int fdx20 = dx20 << 4;
    int fdy01 = dy01 << 4;
    int fdy12 = dy12 << 4;
    int fdy20 = dy20 << 4;

    int c01  = (dy01 * p0.x) - (dx01 * p0.y);
    int c12  = (dy12 * p1.x) - (dx12 * p1.y);
    int c20  = (dy20 * p2.x) - (dx20 * p2.y);

    // Correct for fill convention
    if (dy01 < 0 || (dy01 == 0 && dx01 > 0)) c01 += 1;
    if (dy12 < 0 || (dy12 == 0 && dx12 > 0)) c12 += 1;
    if (dy20 < 0 || (dy20 == 0 && dx20 > 0)) c20 += 1;

    int cy01 = (dx01 * (miny << 4)) + c01 - (dy01 * (minx << 4));
    int cy12 = (dx12 * (miny << 4)) + c12 - (dy12 * (minx << 4));
    int cy20 = (dx20 * (miny << 4)) + c20 - (dy20 * (minx << 4));
    
    glm::vec3 triNormal = glm::normalize(glm::cross(rp1 - rp0, rp2 - rp0));
    float dzdx = -triNormal[0] / triNormal[2];
    float dzdy = -triNormal[1] / triNormal[2];
    float cz = rp0.z + (dzdx * (float(minx) - rp0.x)) + (dzdy * (float(miny) - rp0.y));

    auto& varying0 = get_triangle_varying0(triangle);
    auto& varying1 = get_triangle_varying1(triangle);
    auto& varying2 = get_triangle_varying2(triangle);
    VaryingData interpolatedVaryings;
    VaryingData xgradients;
    VaryingData ygradients;
    interpolatedVaryings.reserve(varying0.size() - 1);
    xgradients.reserve(varying0.size() - 1);
    ygradients.reserve(varying0.size() - 1);
    for (int i = 1; i < varying0.size(); ++i) {
        ShaderVariable& sv0 = varying0[i];
        ShaderVariable& sv1 = varying1[i];
        ShaderVariable& sv2 = varying2[i];
        #define CALC_DELTAS(member) \
                auto A    = (sv2.member - sv0.member) * (rp1.y - rp0.y) - (sv1.member - sv0.member) * (rp2.y - rp0.y);          \
                auto B    = (rp2.x - rp0.x) * (sv1.member - sv0.member) - (rp1.x - rp0.x) * (sv2.member - sv0.member);          \
                float invC     = 1.0f / ((rp1.x - rp0.x) * (rp2.y - rp0.y) - (rp2.x - rp0.x) * (rp1.y - rp0.y));                \
                auto dvdx = -A * invC;                                                                                          \
                auto dvdy = -B * invC;                                                                                          \
                auto cv   = sv0.member + (dvdx * (float(minx) - rp0.x)) + (dvdy * (float(miny) - rp0.y));                       \
                interpolatedVaryings.push_back(cv);                                                                             \
                xgradients.push_back(dvdx);                                                                                     \
                ygradients.push_back(dvdy)                                                                                    
        switch(sv0.type) {
            case Float:  { CALC_DELTAS(f);  break; }
            case Vec4:   { CALC_DELTAS(v4); break; }
            case Vec3:   { CALC_DELTAS(v3); break; }
            case Mat3x3: { CALC_DELTAS(m3); break; }
            case Mat4x4: { CALC_DELTAS(m4); break; }
            default:     { assert(false); break; }
        }
        #undef CALC_DELTAS
    }

    size_t bufferSize = interpolatedVaryings.size() * sizeof(ShaderVariable);
    uint8_t varyingsBuffer[bufferSize];
    ShaderVariable* varyings = reinterpret_cast< ShaderVariable* >(&varyingsBuffer[0]);
    for (int y = miny; y <= maxy; y += 1) {
        int cx01 = cy01;
        int cx12 = cy12;
        int cx20 = cy20;
        float z  = cz;
        std::memcpy(varyings, &interpolatedVaryings[0], bufferSize);
        for (int x = minx; x <= maxx; x += 1) {
            if (cx01 > 0 && cx12 > 0 && cx20 > 0) {
                float currentDepth;
                renderer->depth_buffer().get_pixel(x, y, &currentDepth);
                if (z <= currentDepth) {
                    glm::vec4 color = fsh.ffunc(varyings, fsh.uniforms);
                    color *= glm::vec4(255.0f);
                    uint32_t pixel = (static_cast< uint32_t >(color[3]) << 24) | (static_cast< uint32_t >(color[2]) << 16) | (static_cast< uint32_t >(color[1]) << 8) | static_cast< uint32_t >(color[0]);  
                    renderer->depth_buffer().set_pixel(x, y, &z);
                    renderer->framebuffer().set_pixel(x, y, &pixel);
                }
            }

            cx01 -= fdy01;
            cx12 -= fdy12;
            cx20 -= fdy20;
            z    += dzdx;
            for (int i = 0; i < interpolatedVaryings.size(); ++i) {
                varyings[i] += xgradients[i];
            }
        }

        cy01 += fdx01;
        cy12 += fdx12;
        cy20 += fdx20;
        cz   += dzdy;
        for (int i = 0; i < interpolatedVaryings.size(); ++i) {
            interpolatedVaryings[i] += ygradients[i];
        }
    }
}




