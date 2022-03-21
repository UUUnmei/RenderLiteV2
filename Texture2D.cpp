#include "Texture2D.h"

#include <algorithm>

namespace {
	bool is_power_of_2(int x) {
		int cnt = 0;
		while (x) {
			x = x & (x - 1);
			cnt++;
		}
		return cnt == 1;
	}
}


void Texture2D::GenMipmap(void)
{
	mips.push_back(texture);
	glm::vec4 color[4];
	for (int L = texture->GetWidth(); L > 1; L >>= 1) {
		std::shared_ptr<Image> p = std::make_shared<Image>(L / 2, L / 2, texture->GetBpp());

		for (int i = 0; i < L; i += 2) {
			for (int j = 0; j < L; j += 2) {
				mips.back()->read(i, j, color[0]);
				mips.back()->read(i + 1, j, color[1]);
				mips.back()->read(i, j + 1, color[2]);
				mips.back()->read(i + 1, j + 1, color[3]);

				glm::vec4 avg((color[0] + color[1] + color[2] + color[3]) / 4.0f);
				p->write(i / 2, j / 2, avg);
			}
		}

		mips.push_back(p);
	}
}

bool Texture2D::EnableMipmap(void)
{
	if (texture == nullptr 
	|| texture->GetHeight() != texture->GetWidth() 
	|| !is_power_of_2(texture->GetHeight()) )
		return false;
	is_mipmap_enable = true;
	GenMipmap();
	return true;
}

Texture2D::Texture2D()
	: invW(0), invH(0), texture(nullptr)
{ 
	wmode = WrapMode::Repeat;
	smode = SampleMode::Nearst;
}

Texture2D::Texture2D(const char* file)
	: texture(std::make_shared<Image>(file))
{
	invH = 1.0f / texture->GetHeight();
	invW = 1.0f / texture->GetWidth();
	wmode = WrapMode::Repeat;
	smode = SampleMode::Nearst;
}

namespace {
	const std::vector<glm::vec4> TESTCOLORS{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 1.0f},
		{0.4f, 0.4f, 0.4f, 1.0f}
	};
}

const int Texture2D::GetWidth() const noexcept
{
	return texture->GetWidth();
}

const int Texture2D::GetHeight() const noexcept
{
	return texture->GetHeight();
}

glm::vec4 Texture2D::Sample(float x, float y, float level)
{
	glm::vec4 color(0, 0, 0, 1);

	if (wmode == WrapMode::Repeat) {
		x -= std::floor(x);
		y -= std::floor(y);
	}
	else {
		// clamp to edge
		x = std::min(1.0f - invW, std::max(invW, x));
		y = std::min(1.0f - invH, std::max(invH, y));
	}

	if (is_mipmap_enable) {

		level = std::max(0.0f, level);
		int d1 = int(level);
		int d2 = d1 + 1;
		assert(d1 >= 0);
		assert(d2 < mips.size());
#define MIPMAP_TEST
#ifdef MIPMAP_TEST
		d2 = std::min(d2, 7);
		d1 = std::min(d1, 7);
#else
		d2 = std::min(d2, int(mips.size() - 1));
		d1 = std::min(d1, int(mips.size() - 1));
#endif 
		float a = d2 - level;
		float b = 1 - a;
#ifdef MIPMAP_TEST
		glm::vec4 c1 = TESTCOLORS[d1];
		glm::vec4 c2 = TESTCOLORS[d2];
#else
		glm::vec4 c1 = BilinearSampling(mips[d1], x, y);
		glm::vec4 c2 = BilinearSampling(mips[d2], x, y);
#endif 
		color = c1 * a + c2 * b;
	}
	else if (smode == SampleMode::Nearst) {
		texture->read(x * texture->GetWidth() + 0.5f, (1 - y) * texture->GetHeight() + 0.5f, color);
	}
	else {
		// smode == SampleMode::Bilinear
		color = BilinearSampling(texture, x, y);
	}
	
	return color;
}

glm::vec4 Texture2D::BilinearSampling(std::shared_ptr<Image> src, float x, float y)
{
	/*
			   dx = (a - c11) / (c21 - c11) , dy = (c - c11) / (c12 - c11)
			c11---- a ---- c21
		  dy|		|		|
			|------ c -----	|
			|		|		|
			|		|		|
			c12---- b ---- c22

			a = c11 + (c21 - c11) * dx;
			b = c12 + (c22 - c12) * dx;
			c = a + (b - a) * dy;
		==>
			c = (1 - dx)(1 - dy) * c11 + (1 - dx)dy * c12
			  + (1 - dy)dx * c21 + dxdy * c22

		*/
	x = std::max(0.0f, x) * src->GetWidth() + 0.5f;
	y = std::max(0.0f, 1 - y) * src->GetHeight() + 0.5f;
	int x1 = std::min(src->GetWidth() - 1, (int)x);
	int x2 = std::min(src->GetWidth() - 1, (int)(x + 1));
	int y1 = std::min(src->GetHeight() - 1, (int)y);
	int y2 = std::min(src->GetHeight() - 1, (int)(y + 1));

	glm::vec4 colors[4];
	src->read(x1, y1, colors[0]);
	src->read(x2, y1, colors[1]);
	src->read(x1, y2, colors[2]);
	src->read(x2, y2, colors[3]);

	float dx = (float)(x - x1);// / (x2 - x1); //around 1 
	float dy = (float)(y - y1);// / (y2 - y1);
	glm::vec4 a = colors[0] + (colors[1] - colors[0]) * dx;
	glm::vec4 b = colors[2] + (colors[3] - colors[2]) * dx;
	return a + (b - a) * dy;
}
