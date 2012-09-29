#ifndef JHSR_RENDERER_HPP
#define JHSR_RENDERER_HPP

#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "DefaultRasteriser.hpp"
#include <vector>

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

enum class PolygonWinding
{
	Clockwise,
	CounterClockwise
};

struct Viewport
{
	size_t x, y, w, h;
	float near, far;
};

class Renderer
{
	friend class Pipeline;
	enum : int { MAX_ATTRIBUTES = 10 };

public:

	Renderer();

	~Renderer();

	void draw(size_t start, size_t num);

	void draw_indexed(size_t start, size_t num, int32_t* indices);

	void set_attribute(int index, int components, size_t stride, void* ptr);

	void set_vertex_shader(Shader& vsh);

	void set_fragment_shader(Shader& fsh);

	void set_viewport(size_t x, size_t y, size_t w, size_t h);

	void set_depth_range(float near, float far);

	void set_framebuffer(size_t w, size_t h, size_t bytesPerPixel);

	void set_rasteriser(RasteriserFunc rasterf);

	void set_primitive_topology(PrimitiveTopology topology);

	void set_polygon_winding(PolygonWinding winding);

	Framebuffer& framebuffer();

	Framebuffer const& framebuffer() const;

	Framebuffer& depth_buffer();

	Framebuffer const& depth_buffer() const;

	Viewport const& viewport() const;

	PrimitiveTopology primitive_topology() const;

	PolygonWinding polygon_winding() const;

	int max_attributes() const;

private:

	VertexArray _attributes[MAX_ATTRIBUTES];

	Viewport _viewport;
	Framebuffer* _framebuffer;
	Framebuffer* _depthBuffer;	
	RasteriserFunc _rasterf;
	PrimitiveTopology _primitiveTopology;
	PolygonWinding _winding;

	Shader* _currentVsh;
	Shader* _currentFsh;
};


inline Renderer::Renderer()
: _rasterf(default_rasteriser),
  _primitiveTopology(PrimitiveTopology::TriangleList),
  _winding(PolygonWinding::CounterClockwise) {
	_viewport.near = 0.0f;
	_viewport.far = 1.0f;
}

inline Renderer::~Renderer() {
	if (_framebuffer) delete _framebuffer;
	if (_depthBuffer) delete _depthBuffer;
}

inline void Renderer::set_attribute(int index, int components, size_t stride, void *ptr) {
	assert(index >= 0);
	assert(index < MAX_ATTRIBUTES);
	assert(ptr != nullptr);
	_attributes[index].elementSize = sizeof(float);
	_attributes[index].components = components;
	_attributes[index].stride = stride;
	_attributes[index].vertices = ptr;
}

inline void Renderer::set_vertex_shader(Shader &vsh) {
	_currentVsh = &vsh;
}

inline void Renderer::set_fragment_shader(Shader &fsh) {
	_currentFsh = &fsh;
}

inline void Renderer::set_viewport(size_t x, size_t y, size_t w, size_t h) {
	_viewport.x = x;
	_viewport.y = y;
	_viewport.w = w;
	_viewport.h = h;
}

inline void Renderer::set_depth_range(float near, float far) {
	_viewport.near = std::max(0.0f, std::min(near, 1.0f));
	_viewport.far = std::max(0.0f, std::min(far, 1.0f));
}

inline void Renderer::set_rasteriser(RasteriserFunc rasterf) {
	_rasterf = rasterf;
}

inline void Renderer::set_primitive_topology(PrimitiveTopology topology) {
	_primitiveTopology = topology;
}

inline void Renderer::set_polygon_winding(PolygonWinding winding) {
	_winding = winding;
}

inline Framebuffer& Renderer::framebuffer() {
	return *_framebuffer;
}

inline Framebuffer const& Renderer::framebuffer() const {
	return *_framebuffer;
}

inline Framebuffer& Renderer::depth_buffer() {
	return *_depthBuffer;
}

inline Framebuffer const& Renderer::depth_buffer() const {
	return *_depthBuffer;
}

inline Viewport const& Renderer::viewport() const {
	return _viewport;
}

inline PrimitiveTopology Renderer::primitive_topology() const {
	return _primitiveTopology;
}

inline PolygonWinding Renderer::polygon_winding() const {
	return _winding;
}

inline int Renderer::max_attributes() const {
	return MAX_ATTRIBUTES;
}


#endif // JHSR_RENDERER_HPP
