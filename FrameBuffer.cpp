#include "FrameBuffer.h"



FrameBuffer::FrameBuffer(uint32_t w, uint32_t h)
	: Buffer(w * h * 4), width(w), height(h), bpp(4)
{
	std::cout << "[FRAMEBUFFER] buffer allocate length: " << length << '\n';
}

FrameBuffer::~FrameBuffer()
{
	std::cout << "[FRAMEBUFFER] buffer release length: " << length << '\n';
	// data交给父类析构
}

const int FrameBuffer::GetWidth() const noexcept
{
	return width;
}

const int FrameBuffer::GetHeight() const noexcept
{
	return height;
}

bool FrameBuffer::read(uint32_t x, uint32_t y, glm::vec4& color)
{
	if (x >= width || y >= height) {
		color.r = color.g = color.b = 0;
		color.a = 1;
		return false;
	}
	int p = (y * width + x) * bpp;
	unsigned char cr = data[p + 0];
	unsigned char cg = data[p + 1];
	unsigned char cb = data[p + 2];
	unsigned char ca = data[p + 3];
	color.r = (unsigned int)cr / 255.0f;
	color.g = (unsigned int)cg / 255.0f;
	color.b = (unsigned int)cb / 255.0f;
	color.a = (unsigned int)ca / 255.0f;
	return true;
}

bool FrameBuffer::write(uint32_t x, uint32_t y, const glm::vec4& color)
{
	if (x >= width || y >= height) {
		return false;
	}
	int p = (y * width + x) * bpp;
	data[p + 0] = (unsigned int)(color.r * 255.0f);
	data[p + 1] = (unsigned int)(color.g * 255.0f);
	data[p + 2] = (unsigned int)(color.b * 255.0f);
	data[p + 3] = (unsigned int)(color.a * 255.0f);
	return true;
}

void FrameBuffer::Clear(const glm::vec4& color)
{
	unsigned char r = (unsigned int)(color.r * 255.0f);
	unsigned char g = (unsigned int)(color.g * 255.0f);
	unsigned char b = (unsigned int)(color.b * 255.0f);
	unsigned char a = (unsigned int)(color.a * 255.0f);

	for (int i = 0; i < length; i += bpp) {
		data[i + 0] = r;
		data[i + 1] = g;
		data[i + 2] = b;
		data[i + 3] = a;
	}
}
