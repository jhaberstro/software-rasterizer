#ifndef JHSR_PIPELINE_HPP
#define JHSR_PIPELINE_HPP

#include "Renderer.hpp"
#include "DefaultRasteriser.hpp"
#include <cstring>

struct Renderer;
class Pipeline
{
public:

	Pipeline(Renderer& renderer);

	Renderer const* renderer() const;

	Renderer* renderer();

	void execute(RasteriserFunc rasterf);

private:

	Renderer* _renderer;
};


inline Pipeline::Pipeline(Renderer& renderer)
: _renderer(&renderer) {
}

inline Renderer const* Pipeline::renderer() const {
	return _renderer;
}

inline Renderer* Pipeline::renderer() {
	return _renderer;
}

#endif // JHSR_PIPELINE_HPP