#ifndef JHSR_SHADER_HPP
#define JHSR_SHADER_HPP

#include "VertexArray.hpp"
#include <glm/glm.hpp>
#include <vector>

struct ShaderVariable
{
	ShaderVariable() = default;
	ShaderVariable(ShaderVariable const& sv) : size(4) { std::memcpy(&f, &sv.f, sv.size * sizeof(float)); }
	ShaderVariable& operator=(ShaderVariable const& sv) { std::memcpy(&f, &sv.f, sv.size * sizeof(float)); size = sv.size; return *this; };
	ShaderVariable(float f) : f(f), size(1) {}
	ShaderVariable(glm::vec2 const& v2) : v2(v2), size(2) {}
	ShaderVariable(glm::vec3 const& v3) : v3(v3), size(3) {}
	ShaderVariable(glm::vec4 const& v4) : v4(v4), size(4) {}
	ShaderVariable(glm::mat3x3 const& m3) : m3(m3), size(9) {}
	ShaderVariable(glm::mat4x4 const& m4) : m4(m4), size(16) {}
	ShaderVariable(ShaderVariable&& sv) : m4(std::move(sv.m4)), size(std::move(sv.size)) {}

	ShaderVariable& operator+=(ShaderVariable const& other) {
		assert(size == other.size);
		for (int i = 0; i < size; ++i) {
			arr[i] += other.arr[i];			
		}

		return *this;
	}

	ShaderVariable& operator *= (float x) {
		for (int i = 0; i < size; ++i) {
			arr[i] *= x;
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
		float arr[16];
	};

	int size; 
};

typedef std::vector< ShaderVariable > VaryingData;
// first element of returned vector of VertShaderFunc must be the output position
typedef VaryingData (*VertShaderFunc) (size_t vindex, VertexArray* attributes, std::vector< ShaderVariable > const& uniforms);
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

