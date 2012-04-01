#ifndef JHSR_DEFAULT_RASTERISER_HPP
#define JHSR_DEFAULT_RASTERISER_HPP

#include "Shader.hpp"
#include <vector>
#include <tuple>
#include <glm/glm.hpp>

class Renderer;
typedef std::tuple< glm::vec4, glm::vec4, glm::vec4, VaryingData, VaryingData, VaryingData > TriangleData;
typedef void (*RasteriserFunc) (Renderer* renderer, Shader const& fsh, TriangleData& triangle);
inline glm::vec4& 	get_triangle_vert0(TriangleData& vd) 	{ return std::get< 0 >(vd); }
inline glm::vec4& 	get_triangle_vert1(TriangleData& vd) 	{ return std::get< 1 >(vd); }
inline glm::vec4& 	get_triangle_vert2(TriangleData& vd) 	{ return std::get< 2 >(vd); }
inline VaryingData& get_triangle_varying0(TriangleData& vd) { return std::get< 3 >(vd); }
inline VaryingData& get_triangle_varying1(TriangleData& vd) { return std::get< 4 >(vd); }
inline VaryingData& get_triangle_varying2(TriangleData& vd) { return std::get< 5 >(vd); }

void default_rasteriser(Renderer* renderer, Shader const& fsh, TriangleData& triangle);

#endif // JHSR_DEFAULT_RASTERISER_HPP