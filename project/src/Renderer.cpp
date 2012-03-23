#include "Renderer.hpp"
#include <cassert>

void Renderer::set_framebuffer(size_t w, size_t h, size_t bytesPerPixel) {
	if (_framebuffer != nullptr) {
		delete _framebuffer;
	}

	if (_depthBuffer != nullptr) {
		delete _depthBuffer;
	}

	_framebuffer = new Framebuffer(w, h, bytesPerPixel);
	_depthBuffer = new Framebuffer(w, h, 4);
}

void Renderer::draw(const std::vector< VertexArray > const& attributes, size_t start, size_t num) {
	assert(_currentVsh != nullptr);
	assert(_currentFsh != nullptr);
	DrawCall* newDrawCall = new DrawCall;
	newDrawCall->attributeData = &attributes;
	newDrawCall->vertexShader = _currentVsh;
	newDrawCall->fragmentShader = _currentFsh;
	newDrawCall->startVert = start;
	newDrawCall->numVerts = num;		
	drawCalls.push(newDrawCall);
}