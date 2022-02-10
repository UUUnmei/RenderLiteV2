#pragma once

#include "Image.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Texture2D
{
	std::shared_ptr<Image> texture;  // 同一张纹理就别拷贝了
	float invW;
	float invH;

	bool is_mipmap_enable{ false };
	
	void GenMipmap(void);
public:
	std::vector<std::shared_ptr<Image>> mips;
	enum class WrapMode {
		Repeat,
		ClampToEdge
	};
	enum class SampleMode {
		Nearst,
		Bilinear,
		// trilinear if enable Mipmap
	};
	WrapMode wmode;
	SampleMode smode;
	
	bool EnableMipmap(void);

public:
	Texture2D();
	//Texture2D(const Texture2D&);
	//Texture2D& operator=(const Texture2D&); 
	Texture2D(const Texture2D&) = default;
	Texture2D& operator=(const Texture2D&) = default;
	Texture2D(const char* file);

	const int GetWidth() const noexcept;
	const int GetHeight() const noexcept;

	// sample function
	// level refer to mipmap level
	glm::vec4 Sample(float x, float y, float level = 0.0f);

private:
	glm::vec4 BilinearSampling(std::shared_ptr<Image> src, float x, float y);

};

