#include "DefaultRasteriser.hpp"
#include "Renderer.hpp"
#include <tuple>
#include <array>
#include <cmath>
#include <glm/gtc/swizzle.hpp>
#include "FixedPointMath.hpp"

// Ax + By + Cz + D = 0, where (A, B, C) is normal to the tri-plane
// -> z = -A/C*x - B/C*y - D
template< typename T >
inline std::tuple< T, T, T > calculate_gradients(T const& u0, T const& u1, T const& u2, glm::vec4 const& p0, glm::vec4 const& p1, glm::vec4 const& p2, int minx, int miny) {
    auto u20  = u2 - u0,     u10 = u1 - u0;
    auto x20  = p2.x - p0.x, x10 = p1.x - p0.x;
    auto y20  = p2.y - p0.y, y10 = p1.y - p0.y;
    auto A    = u20 * y10 - u10 * y20;
    auto B    = x20 * u10 - x10 * u20;
    auto invC = 1.0f / ((x10) * y20 - (x20) * y10);           
    auto dudx = -A * invC;                                                                                
    auto dudy = -B * invC;                                                                                
    auto cu   = u0 + (dudx * (minx - p0.x)) + (dudy * (miny - p0.y));   
    return std::make_tuple(dudx, dudy, cu);                        
}

#define CALC_DELTAS(u0, u1, u2, rp0, rp1, rp2, minx, miny) {                                                            \
        decltype(u0) dvdx, dvdy, cv;                                                                            \
        std::tie(dvdx, dvdy, cv) = calculate_gradients(u0, u1, u2, rp0, rp1, rp2, minx, miny);                          \
        interpolatedVaryings.push_back(cv);                                                                             \
        xgradients.push_back(dvdx);                                                                                     \
        ygradients.push_back(dvdy);                                                                                     \
    }

void default_rasteriser(Renderer* renderer, Shader const& fsh, TriangleData& triangle) {
    enum { P = 4 };
    glm::vec4& p0 = get_triangle_vert0(triangle);
    glm::vec4& p1 = get_triangle_vert1(triangle);
    glm::vec4& p2 = get_triangle_vert2(triangle);
    glm::ivec2 ip0(iround(p0.x * fixed_base< P >()), iround(p0.y * fixed_base< P >()));
    glm::ivec2 ip1(iround(p1.x * fixed_base< P >()), iround(p1.y * fixed_base< P >()));
    glm::ivec2 ip2(iround(p2.x * fixed_base< P >()), iround(p2.y * fixed_base< P >()));
    int minx = iround(std::max(0.0f, std::min(std::min(p0.x, p1.x), p2.x)));
    int miny = iround(std::max(0.0f, std::min(std::min(p0.y, p1.y), p2.y)));
    int maxx = iround(std::min(float(renderer->framebuffer().width() - 1),  std::max(std::max(p0.x, p1.x), p2.x)));
    int maxy = iround(std::min(float(renderer->framebuffer().height() - 1), std::max(std::max(p0.y, p1.y), p2.y)));

    // All derived from:
    //  float edge01 = (dx01 * (y - p0.y)) - (dy01 * (x - p0.x));
    //  float edge12 = (dx12 * (y - p1.y)) - (dy12 * (x - p1.x));
    //  float edge20 = (dx20 * (y - p2.y)) - (dy20 * (x - p2.x));
    int dx01 = (ip1.x - ip0.x);
    int dx12 = (ip2.x - ip1.x);
    int dx20 = (ip0.x - ip2.x);
    int dy01 = (ip1.y - ip0.y);
    int dy12 = (ip2.y - ip1.y);
    int dy20 = (ip0.y - ip2.y);

    int c01 = fixed_mult< P >(dx01, -ip0.y) + fixed_mult< P >(dy01, ip0.x);
    int c12 = fixed_mult< P >(dx12, -ip1.y) + fixed_mult< P >(dy12, ip1.x);
    int c20 = fixed_mult< P >(dx20, -ip2.y) + fixed_mult< P >(dy20, ip2.x);

    // Correct for fill convention
    if (dy01 < 0 || (dy01 == 0 && dx01 > 0)) c01 += 1;
    if (dy12 < 0 || (dy12 == 0 && dx12 > 0)) c12 += 1;
    if (dy20 < 0 || (dy20 == 0 && dx20 > 0)) c20 += 1;

    int cy01 = fixed_mult< P >(dx01, (miny << P)) + fixed_mult< P >(dy01, -(minx << P)) + c01;
    int cy12 = fixed_mult< P >(dx12, (miny << P)) + fixed_mult< P >(dy12, -(minx << P)) + c12;
    int cy20 = fixed_mult< P >(dx20, (miny << P)) + fixed_mult< P >(dy20, -(minx << P)) + c20;

    // Calculate gradient values for z, 1/w, and all varyings
    float dzdx, dzdy, cz;
    float dwdx, dwdy, cw;
    std::tie(dzdx, dzdy, cz) = calculate_gradients(p0.z, p1.z, p2.z, p0, p1, p2, minx, miny);
    std::tie(dwdx, dwdy, cw) = calculate_gradients(p0.w, p1.w, p2.w, p0, p1, p2, minx, miny);

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
        switch(sv0.type) {
            case Float:  { CALC_DELTAS(sv0.f  * p0.w, sv1.f  * p1.w, sv2.f  * p2.w, p0, p1, p2, minx, miny); break; }
            case Vec2:   { CALC_DELTAS(sv0.v2 * p0.w, sv1.v2 * p1.w, sv2.v2 * p2.w, p0, p1, p2, minx, miny); break; }
            case Vec3:   { CALC_DELTAS(sv0.v3 * p0.w, sv1.v3 * p1.w, sv2.v3 * p2.w, p0, p1, p2, minx, miny); break; }
            case Vec4:   { CALC_DELTAS(sv0.v4 * p0.w, sv1.v4 * p1.w, sv2.v4 * p2.w, p0, p1, p2, minx, miny); break; }
            case Mat3x3: { CALC_DELTAS(sv0.m3 * p0.w, sv1.m3 * p1.w, sv2.m3 * p2.w, p0, p1, p2, minx, miny); break; }
            case Mat4x4: { CALC_DELTAS(sv0.m4 * p0.w, sv1.m4 * p1.w, sv2.m4 * p2.w, p0, p1, p2, minx, miny); break; }
            default:     { assert(false); break; }
        }
    }

    size_t bufferSize = interpolatedVaryings.size() * sizeof(ShaderVariable);
    uint8_t varyingsBuffer[bufferSize];
    ShaderVariable* varyings = reinterpret_cast< ShaderVariable* >(&varyingsBuffer[0]);
    for (int y = miny; y <= maxy; y += 1) {
        int cx01 = cy01;
        int cx12 = cy12;
        int cx20 = cy20;
        float z  = cz;
        float w  = cw;
        std::memcpy(varyings, &interpolatedVaryings[0], bufferSize);
        for (int x = minx; x <= maxx; x += 1) {
            if (cx01 > 0 && cx12 > 0 && cx20 > 0) {
                float currentDepth;
                renderer->depth_buffer().get_pixel(x, y, &currentDepth);
                if (z <= currentDepth) {
                    for (int i = 0; i < interpolatedVaryings.size(); ++i) varyings[i] *= 1.0f / w;
                    glm::vec4 color = fsh.ffunc(varyings, fsh.uniforms);
                    for (int i = 0; i < interpolatedVaryings.size(); ++i) varyings[i] *= w;
                    color *= glm::vec4(255.0f);
                    uint32_t pixel = (static_cast< uint32_t >(color[3]) << 24) | (static_cast< uint32_t >(color[2]) << 16) | (static_cast< uint32_t >(color[1]) << 8) | static_cast< uint32_t >(color[0]);  
                    renderer->depth_buffer().set_pixel(x, y, &z);
                    renderer->framebuffer().set_pixel(x, y, &pixel);
                }
            }

            cx01 -= dy01;
            cx12 -= dy12;
            cx20 -= dy20;
            z    += dzdx;
            w    += dwdx;
            for (int i = 0; i < interpolatedVaryings.size(); ++i) {
                varyings[i] += xgradients[i];
            }
        }

        cy01 += dx01;
        cy12 += dx12;
        cy20 += dx20;
        cz   += dzdy;
        cw   += dwdy;
        for (int i = 0; i < interpolatedVaryings.size(); ++i) {
            interpolatedVaryings[i] += ygradients[i];
        }
    }
}




