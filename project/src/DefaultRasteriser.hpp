#ifndef JHSR_DEFAULT_RASTERISER_HPP
#define JHSR_DEFAULT_RASTERISER_HPP

#include "Context.hpp"
#include <vector>
#include <glm/glm.hpp>

void default_rasteriser(StateContext& ctx, Shader const& fsh, TriangleData& triangle);

#endif // JHSR_DEFAULT_RASTERISER_HPP