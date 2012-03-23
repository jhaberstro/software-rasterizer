#ifndef JHSR_RENDERER_HPP
#define JHSR_RENDERER_HPP

#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <vector>
#include <queue>

struct Viewport
{
	size_t x, y, w, h;
	float near, far;
};

struct Renderer
{
	friend class Pipeline;

public:

	Renderer();

	~Renderer();

	void draw(std::vector< VertexArray > const& attributes, size_t start, size_t num);

	void set_vertex_shader(Shader& vsh);

	void set_fragment_shader(Shader& fsh);

	void set_viewport(size_t x, size_t y, size_t w, size_t h);

	void set_depth_range(float near, float far);

	void set_framebuffer(size_t w, size_t h, size_t bytesPerPixel);

	Framebuffer& framebuffer();

	Framebuffer const& framebuffer() const;

	Framebuffer& depth_buffer();

	Framebuffer const& depth_buffer() const;

	Viewport const& viewport() const;

protected:

	struct DrawCall
	{
		Shader const* vertexShader;
		Shader const* fragmentShader;
		std::vector< VertexArray > const* attributeData;
		size_t startVert;
		size_t numVerts;
	};


private:

	Viewport _viewport;
	Framebuffer* _framebuffer;
	Framebuffer* _depthBuffer;
	std::queue< DrawCall* > drawCalls;

	Shader* _currentVsh;
	Shader* _currentFsh;
};


inline Renderer::Renderer() {
	_viewport.near = 0.0f;
	_viewport.far = 1.0f;
}

inline Renderer::~Renderer() {
	if (_framebuffer) delete _framebuffer;
	if (_depthBuffer) delete _depthBuffer;
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
#endif // JHSR_RENDERER_HPP