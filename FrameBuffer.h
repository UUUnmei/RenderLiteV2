#pragma once

#include "Buffer.h"

#include <glm/glm.hpp>

#include <stdint.h>

class FrameBuffer final : public Buffer<unsigned char>
{
	int width;
	int height;
	int bpp;  // byte per pixel

public:
	FrameBuffer(uint32_t w, uint32_t h);
	FrameBuffer(const FrameBuffer&) = delete;   //similar but different with Image
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	~FrameBuffer();

	const int GetWidth() const noexcept;
	const int GetHeight() const noexcept;
	bool read(uint32_t x, uint32_t y, glm::vec4& color);
	bool write(uint32_t x, uint32_t y, const glm::vec4& color);
	void Clear(const glm::vec4& color);

};
