#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cassert>
#include <iostream>

Image::Image()
	: Buffer(), width(0), height(0), bpp(0)
{  }

Image::Image(uint32_t w, uint32_t h, uint32_t b)
	: Buffer(w* h * b), width(w), height(h), bpp(b)
{
	std::cout << "[IMAGE] buffer allocate length: " << length << '\n';
}

Image::Image(const char* file)
	: Buffer()
{ 
	// make sure bpp of loaded image is 4
	// while the return value of bpp just "report original components"(line 3978)
	data = stbi_load(file, &width, &height, &bpp, STBI_rgb_alpha); 
	assert(data);
	bpp = STBI_rgb_alpha;
	length = width * height * STBI_rgb_alpha;
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

const int Image::GetBpp() const noexcept
{
	return bpp;
}

bool Image::read(uint32_t x, uint32_t y, glm::vec4& color)
{
#ifdef _DEBUG
	if (x >= width || y >= height) {
		color.r = color.g = color.b = 0;
		color.a = 1;
		return false;
	}
#endif
	int p = (y * width + x) * bpp;
	unsigned char cr = data[p + 0];
	unsigned char cg = data[p + 1];
	unsigned char cb = data[p + 2];
	unsigned char ca = data[p + 3];
	color.r = (unsigned int)cr * INV255;
	color.g = (unsigned int)cg * INV255;
	color.b = (unsigned int)cb * INV255;
	color.a = (unsigned int)ca * INV255;
	return true;
} 

bool Image::write(uint32_t x, uint32_t y, const glm::vec4& color)
{
#ifdef _DEBUG
	if (x >= width || y >= height) {
		return false;
	}
#endif
	int p = (y * width + x) * bpp;
	data[p + 0] = (unsigned int)(color.r * 255.0f);
	data[p + 1] = (unsigned int)(color.g * 255.0f);
	data[p + 2] = (unsigned int)(color.b * 255.0f);
	data[p + 3] = (unsigned int)(color.a * 255.0f);
	return true;
}

void Image::Dump(const char* outputname, int w, int h, int bpp, const unsigned char* data)
{
	stbi_write_png(outputname, w, h, bpp, data, 0);
	std::cout << "DUMP success " << outputname << '\n';
}


Image::~Image()
{
	if (data)
		stbi_image_free(data);
	data = nullptr;
	std::cout << "[IMAGE] buffer release length: " << length << '\n';
}

