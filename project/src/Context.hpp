#ifndef JHSR_CONTEXT_HPP
#define JHSR_CONTEXT_HPP

#include "Framebuffer.hpp"
#include <glm/glm.hpp>
#include <queue>
#include <vector>
#include <tuple>
#include <cstring>

struct Viewport
{
	size_t x, y, w, h;
	float near, far;
};

enum ShaderDataType : unsigned int
{
	Float,
	Vec3,
	Vec4,
	Mat3x3,
	Mat4x4
};

struct VertexArray
{
	VertexArray() = default;
	VertexArray(VertexArray const& va) : components(va.components), stride(va.stride), vertices(va.vertices) {}
	VertexArray& operator=(VertexArray const& va) { components = va.components; stride = va.stride; vertices = va.vertices; return *this; };
	VertexArray(VertexArray&& va) : components(std::move(va.components)), stride(std::move(va.stride)), vertices(std::move(va.vertices)) {}

	size_t components;
	size_t stride;
	void* vertices;
};

struct ShaderVariable
{
	ShaderVariable() = default;
	ShaderVariable(ShaderVariable const& sv) : m4(sv.m4), type(sv.type) {}
	ShaderVariable& operator=(ShaderVariable const& sv) { m4 = sv.m4; type = sv.type; return *this; };
	ShaderVariable(float f) : f(f), type(Float) {}
	ShaderVariable(glm::vec3 const& v3) : v3(v3), type(Vec3) {}
	ShaderVariable(glm::vec4 const& v4) : v4(v4), type(Vec4) {}
	ShaderVariable(glm::mat3x3 const& m3) : m3(m3), type(Mat3x3) {}
	ShaderVariable(glm::mat4x4 const& m4) : m4(m4), type(Mat4x4) {}
	ShaderVariable(ShaderVariable&& sv) : m4(std::move(sv.m4)), type(std::move(sv.type)) {}

	union {
		float f;
		glm::vec3 v3;
		glm::vec4 v4;
		glm::mat3x3 m3;
		glm::mat4x4 m4;
	};

	ShaderDataType type; 
};

using VaryingData = std::vector< ShaderVariable >;
// first element of returned vector of VertShaderFunc must be the output position
using VertShaderFunc = VaryingData (*) (size_t vindex, std::vector< VertexArray >& attributes, std::vector< ShaderVariable >& uniforms);
using FragShaderFunc = glm::vec4 (*) (std::vector< ShaderVariable >& varyings, std::vector< ShaderVariable >& uniforms);

// The thought is that since author of the shader is responsible
// for determining how uniform data is layed out, providing a
// straight list of shader variables is reasonable.
struct Shader
{


	std::vector< ShaderVariable > uniforms;
	union {
		VertShaderFunc vfunc;
		FragShaderFunc ffunc;
	};
};

struct DrawCall
{
	Shader vertexShader;
	Shader fragmentShader;
	std::vector< VertexArray > attributeData;
	size_t startVert;
	size_t numVerts;
};

class Framebuffer;
class Pipeline;
struct StateContext
{
	friend class Pipeline;

public:

	StateContext();

	~StateContext();

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

	Viewport& viewport();

	Viewport const& viewport() const;

private:

	Viewport _viewport;
	Framebuffer* _framebuffer;
	Framebuffer* _depthBuffer;
	std::queue< DrawCall* > drawCalls;

	Shader* _currentVsh;
	Shader* _currentFsh;
};

typedef std::tuple<glm::vec3, glm::vec3, glm::vec3, VaryingData, VaryingData, VaryingData > TriangleData;
using RasteriserFunc = void (*) (StateContext& ctx, Shader& fsh, TriangleData& triangle);

inline glm::vec3& get_triangle_vert0(TriangleData& vd) { return std::get< 0 >(vd); }
inline glm::vec3& get_triangle_vert1(TriangleData& vd) { return std::get< 1 >(vd); }
inline glm::vec3& get_triangle_vert2(TriangleData& vd) { return std::get< 2 >(vd); }
inline VaryingData& get_triangle_varying0(TriangleData& vd) { return std::get< 3 >(vd); }
inline VaryingData& get_triangle_varying1(TriangleData& vd) { return std::get< 4 >(vd); }
inline VaryingData& get_triangle_varying2(TriangleData& vd) { return std::get< 5 >(vd); }

inline StateContext::StateContext() {
	_viewport.near = 0.0f;
	_viewport.far = 1.0f;
}

inline StateContext::~StateContext() {
	if (_framebuffer) delete _framebuffer;
	if (_depthBuffer) delete _depthBuffer;
}

inline void StateContext::set_vertex_shader(Shader &vsh) {
	_currentVsh = &vsh;
}

inline void StateContext::set_fragment_shader(Shader &fsh) {
	_currentFsh = &fsh;
}

inline void StateContext::set_viewport(size_t x, size_t y, size_t w, size_t h) {
	_viewport.x = x;
	_viewport.y = y;
	_viewport.w = w;
	_viewport.h = h;
}

inline void StateContext::set_depth_range(float near, float far) {
	_viewport.near = std::max(0.0f, std::min(near, 1.0f));
	_viewport.far = std::max(0.0f, std::min(far, 1.0f));
}

inline Framebuffer& StateContext::framebuffer() {
	return *_framebuffer;
}

inline Framebuffer const& StateContext::framebuffer() const {
	return *_framebuffer;
}

inline Framebuffer& StateContext::depth_buffer() {
	return *_depthBuffer;
}

inline Framebuffer const& StateContext::depth_buffer() const {
	return *_depthBuffer;
}

inline Viewport& StateContext::viewport() {
	return _viewport;
}

inline Viewport const& StateContext::viewport() const {
	return _viewport;
}

#endif // JHSR_CONTEXT_HPP
