#pragma once

#include "Buffer.h"

#include <glm/glm.hpp>

#include <stdint.h>

class Image final : public Buffer<unsigned char>
{
	int width;
	int height;
	int bpp;  // byte per pixel

public:
	Image();
	Image(const char* file);
	Image(const Image&) = default;  // image�Լ�ֻ�Ǽ򵥿���һ��w,h,bpp ֮�󽻸����࿽������
	Image& operator=(const Image&) = default;
	~Image();

	const int GetWidth() const noexcept;
	const int GetHeight() const noexcept;
	bool read(uint32_t x, uint32_t y, glm::vec4& color);
	bool write(uint32_t x, uint32_t y, const glm::vec4& color);

	static void Dump(const char* outputname, int w, int h, int bpp, const unsigned char* data);

};

