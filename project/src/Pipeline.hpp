#ifndef JHSR_PIPELINE_HPP
#define JHSR_PIPELINE_HPP

#include "Context.hpp"
#include <cstring>

struct StateContext;
class Pipeline
{
public:

	StateContext const* context() const;

	StateContext* context();

	void execute(RasteriserFunc rasterf);

private:

	StateContext* _context;
};

StateContext const* Pipeline::context() const {
	return _context;
}

StateContext* Pipeline::context() {
	return _context;
}

#endif // JHSR_PIPELINE_HPP