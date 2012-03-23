#ifndef JHSR_FRAMEBUFFER_HPP
#define JHSR_FRAMEBUFFER_HPP

#include <stdint.h>
#include <cstring>
#include <cassert>
#include <cstdio>

class Framebuffer
{
public:

	Framebuffer(size_t width, size_t height, size_t bytesPerPixel);

	~Framebuffer();

	size_t width() const;

	size_t height() const;

	size_t bytes_per_pixel() const;

	void clear(void const* value);

	void set_pixel(size_t x, size_t y, void const* pixel);

	void set_row(size_t row, void const* rowPixels);

	void get_pixel(size_t x, size_t y, void* result) const;

	void const* pixels() const;

private:

	uint8_t* _pixels;
	size_t _width, _height, _bytesPerPixel;
};


inline Framebuffer::Framebuffer(size_t width, size_t height, size_t bytesPerPixel)
: _width(width), _height(height), _bytesPerPixel(bytesPerPixel) {
	assert(width > 0);
	assert(height > 0);
	_pixels = new uint8_t[width * height * bytesPerPixel];
	std::memset(_pixels, 0, sizeof(uint8_t) * width * height * bytesPerPixel);
}

inline Framebuffer::~Framebuffer() {
	delete [] _pixels;
}

inline size_t Framebuffer::width() const {
	return _width;
}

inline size_t Framebuffer::height() const {
	return _height;
}

inline size_t Framebuffer::bytes_per_pixel() const {
	return _bytesPerPixel;
}

inline void Framebuffer::clear(void const* value) {
	for (size_t y = 0; y < _height; ++y) {
		for (size_t x = 0; x < _width; ++x) {
			this->set_pixel(x, y, value);
		}
	}
}

inline void Framebuffer::set_pixel(size_t x, size_t y, void const* pixel) {
	assert(x >= 0 && x < _width);
	assert(y >= 0 && y < _height);
	std::memcpy(_pixels + (y * _width * _bytesPerPixel) + (x * _bytesPerPixel), pixel, _bytesPerPixel);
}

inline void Framebuffer::set_row(size_t row, void const* rowPixels) {
	assert(row >= 0 && row < _height);
	std::memcpy(_pixels + (row * _width * _bytesPerPixel), rowPixels, _width * _bytesPerPixel);
}

inline void Framebuffer::get_pixel(size_t x, size_t y, void* result) const {
	assert(x >= 0 && x < _width);
	assert(y >= 0 && y < _height);
	std::memcpy(result, _pixels + (y * _width * _bytesPerPixel) + (x * _bytesPerPixel), _bytesPerPixel);
}


inline void const* Framebuffer::pixels() const {
	return _pixels;
}

#endif // JHSR_FRAMEBUFFER