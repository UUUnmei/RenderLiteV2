#include "Texture2D.h"


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

//Texture2D::Texture2D(const Texture2D& rhs)
//	: texture(rhs.texture)
//{ }
//
//Texture2D& Texture2D::operator=(const Texture2D& rhs)    // Éî¿½±´¡£¡£¡£
//{
//	texture = std::make_shared<Image>(*rhs.texture);
//	return *this;
//}

Texture2D::Texture2D(const char* file)
	: texture(std::make_shared<Image>(file))
{
	invH = 1.0f / texture->GetHeight();
	invW = 1.0f / texture->GetWidth();
	wmode = WrapMode::Repeat;
	smode = SampleMode::Nearst;
}

glm::vec4 Texture2D::Sample(float x, float y)
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

	if (smode == SampleMode::Nearst) {
		texture->read(x * texture->GetWidth() + 0.5f, (1 - y) * texture->GetHeight() + 0.5f, color);
	}
	else if (smode == SampleMode::Bilinear) {

	}
	else {
		// mipmap

	}
	
	return color;
}
