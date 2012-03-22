#ifndef JHSR_FRAMEBUFFER_HPP
#define JHSR_FRAMEBUFFER_HPP

#include <stdint.h>
#include <cstring>
#include <cassert>

class Framebuffer
{
public:

	Framebuffer(size_t width, size_t height, size_t bytesPerPixel);

	~Framebuffer();

	size_t width() const;

	size_t height() const;

	size_t bytes_per_pixel() const;

	void set_pixel(size_t x, size_t y, uint8_t const* pixel);

	void set_row(size_t row, uint8_t const* rowPixels);

	void get_pixel(size_t x, size_t y, uint8_t* result) const;

	uint8_t const* pixels() const;

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

inline void Framebuffer::set_pixel(size_t x, size_t y, uint8_t const* pixel) {
	std::memcpy(_pixels + (y * _width * _bytesPerPixel) + (x * _bytesPerPixel), pixel, _bytesPerPixel);
}

inline void Framebuffer::set_row(size_t row, uint8_t const* rowPixels) {
	std::memcpy(_pixels + (row * _width * _bytesPerPixel), rowPixels, _width * _bytesPerPixel);
}

inline void Framebuffer::get_pixel(size_t x, size_t y, uint8_t *result) const {
	std::memcpy(result, _pixels + (y * _width * _bytesPerPixel) + (x * _bytesPerPixel), _bytesPerPixel);
}


inline uint8_t const* Framebuffer::pixels() const {
	return _pixels;
}

#endif // JHSR_FRAMEBUFFER