#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <iostream>

Image::Image()
	: Buffer(), width(0), height(0), bpp(0)
{  }

Image::Image(const char* file)
	: Buffer()
{ 
	//data = stbi_load(file, &width, &height, &bpp, 4); // FAIL to make sure bpp==4
	data = stbi_load(file, &width, &height, &bpp, 0);
	length = width * height * bpp;
	assert(length == (uint64_t)length); // 应该不会超32位的范围，在这检测其实好像有点晚了。。
	
	std::cout << "[IMAGE] buffer allocate length: " << length << '\n';
}

const int Image::GetWidth() const noexcept
{
	return width;
}

const int Image::GetHeight() const noexcept
{
	return height;
}

bool Image::read(uint32_t x, uint32_t y, glm::vec4& color)
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
	unsigned char ca = bpp == 4 ? data[p + 3] : 0xFF;
	color.r = (unsigned int)cr / 255.0f;
	color.g = (unsigned int)cg / 255.0f;
	color.b = (unsigned int)cb / 255.0f;
	color.a = (unsigned int)ca / 255.0f;
	return true;
} 

bool Image::write(uint32_t x, uint32_t y, const glm::vec4& color)
{
	if (x >= width || y >= height) {
		return false;
	}
	int p = (y * width + x) * bpp;
	data[p + 0] = (unsigned int)(color.r * 255.0f);
	data[p + 1] = (unsigned int)(color.g * 255.0f);
	data[p + 2] = (unsigned int)(color.b * 255.0f);
	if(bpp == 4)
		data[p + 3] = (unsigned int)(color.a * 255.0f);
	return true;
}


Image::~Image()
{
	if (data)
		stbi_image_free(data);
	data = nullptr;
	std::cout << "[IMAGE] buffer release length: " << length << '\n';
}

