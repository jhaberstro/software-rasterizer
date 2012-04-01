#ifndef JHSR_SHADER_HPP
#define JHSR_SHADER_HPP

#include "VertexArray.hpp"
#include <glm/glm.hpp>
#include <vector>

enum ShaderDataType : unsigned int
{
	Float  = 0,
	Vec2   = 1,
	Vec3   = 2,
	Vec4   = 3,
	Mat3x3 = 4,
	Mat4x4 = 5
};

struct ShaderVariable
{
	ShaderVariable() = default;
	ShaderVariable(ShaderVariable const& sv) : m4(sv.m4), type(sv.type) {}
	ShaderVariable& operator=(ShaderVariable const& sv) { m4 = sv.m4; type = sv.type; return *this; };
	ShaderVariable(float f) : f(f), type(Float) {}
	ShaderVariable(glm::vec2 const& v2) : v2(v2), type(Vec2) {}
	ShaderVariable(glm::vec3 const& v3) : v3(v3), type(Vec3) {}
	ShaderVariable(glm::vec4 const& v4) : v4(v4), type(Vec4) {}
	ShaderVariable(glm::mat3x3 const& m3) : m3(m3), type(Mat3x3) {}
	ShaderVariable(glm::mat4x4 const& m4) : m4(m4), type(Mat4x4) {}
	ShaderVariable(ShaderVariable&& sv) : m4(std::move(sv.m4)), type(std::move(sv.type)) {}

	ShaderVariable& operator+=(ShaderVariable const& other) {
		assert(type == other.type);
		static int sizes[] = {1, 2, 3, 4, 9, 16};
		int size = sizes[type];
		float* pf = &f;
		float const* pfother = &other.f;
		for (int i = 0; i < size; ++i) {
			pf[i] += pfother[i];
		}

		return *this;
	}

	ShaderVariable& operator *= (float x) {
		static int sizes[] = {1, 2, 3, 4, 9, 16};
		int size = sizes[type];
		float* pf = &f;
		for (int i = 0; i < size; ++i) {
			pf[i] *= x;
		}

		return *this;
	}

	union {
		float f;
		glm::vec2 v2;
		glm::vec3 v3;
		glm::vec4 v4;
		glm::mat3x3 m3;
		glm::mat4x4 m4;
	};

	ShaderDataType type; 
};

typedef std::vector< ShaderVariable > VaryingData;
// first element of returned vector of VertShaderFunc must be the output position
typedef VaryingData (*VertShaderFunc) (size_t vindex, std::vector< VertexArray > const& attributes, std::vector< ShaderVariable > const& uniforms);
typedef glm::vec4 (*FragShaderFunc) (ShaderVariable* varyings, std::vector< ShaderVariable > const& uniforms);

// The thought is that since author of the shader is responsible
// for determining how uniform data is layed out, providing a
// straight list of shader variables is reasonable.
struct Shader
{
	Shader(VertShaderFunc vsh) : vfunc(vsh) {}
	Shader(FragShaderFunc fsh) : ffunc(fsh) {}

	std::vector< ShaderVariable > uniforms;
	union {
		VertShaderFunc vfunc;
		FragShaderFunc ffunc;
	};
};

#endif // JHSR_SHADER_HPP

