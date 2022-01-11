#pragma once

#include "Buffer.h"
#include <glm/fwd.hpp>

class DepthBuffer : public Buffer<float>
{
	int width;
	int height;
public:
	DepthBuffer(uint32_t w, uint32_t h);
	DepthBuffer(const DepthBuffer&) = delete;   //similar but different with Image
	DepthBuffer& operator=(const DepthBuffer&) = delete;
	~DepthBuffer();

	const int GetWidth() const noexcept;
	const int GetHeight() const noexcept;
	
	bool TryUpdate(uint32_t x, uint32_t y, float newZ);
	void clear();
};

