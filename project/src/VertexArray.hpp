#ifndef JHSR_VERTEXARRAY_HPP
#define JHSR_VERTEXARRAY_HPP

#include <algorithm>

struct VertexArray
{
	VertexArray() = default;
	VertexArray(size_t c, size_t s, void* v) : components(c), stride(s), vertices(v) {}
	VertexArray(VertexArray const& va) : components(va.components), stride(va.stride), vertices(va.vertices) {}
	VertexArray& operator=(VertexArray const& va) { components = va.components; stride = va.stride; vertices = va.vertices; return *this; };
	VertexArray(VertexArray&& va) : components(std::move(va.components)), stride(std::move(va.stride)), vertices(std::move(va.vertices)) {}

	void* index(size_t i) {
		return (uint8_t*)(vertices) + (elementSize * components * i) + (stride * i);	
	}

	size_t elementSize;
	size_t components;
	size_t stride;
	void* vertices;
};

#endif // JHSR_VERTEXARRAY_HPP