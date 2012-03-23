#ifndef JHSR_PIPELINE_HPP
#define JHSR_PIPELINE_HPP

#include "Renderer.hpp"
#include "DefaultRasteriser.hpp"
#include <cstring>

struct Renderer;
class Pipeline
{
public:

	Pipeline(Renderer& ctx);

	Renderer const* context() const;

	Renderer* context();

	void execute(RasteriserFunc rasterf);

private:

	Renderer* _context;
};


inline Pipeline::Pipeline(Renderer& ctx)
: _context(&ctx) {
}

inline Renderer const* Pipeline::context() const {
	return _context;
}

inline Renderer* Pipeline::context() {
	return _context;
}

#endif // JHSR_PIPELINE_HPP