#include "Context.hpp"
#include "Framebuffer.hpp"
#include <cassert>

void StateContext::set_framebuffer(size_t w, size_t h, size_t bytesPerPixel) {
	if (_framebuffer != nullptr) {
		delete _framebuffer;
	}

	if (_depthBuffer != nullptr) {
		delete _depthBuffer;
	}

	_framebuffer = new Framebuffer(w, h, bytesPerPixel);
	_depthBuffer = new Framebuffer(w, h, 4);
}

void StateContext::draw(const std::vector<VertexArray> &attributes, size_t start, size_t num) {
	DrawCall* newDrawCall = new DrawCall;
	newDrawCall->attributeData = attributes;
	newDrawCall->vertexShader = std::move(*_currentVsh);
	newDrawCall->fragmentShader = std::move(*_currentFsh);
	newDrawCall->startVert = start;
	newDrawCall->numVerts = num;		
	drawCalls.push(newDrawCall);
}
