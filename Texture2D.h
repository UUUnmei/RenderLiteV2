#pragma once

#include "Image.h"

#include <glm/glm.hpp>

#include <memory>

class Texture2D
{
	std::shared_ptr<Image> texture;  // 同一张纹理就别拷贝了
	float invW;
	float invH;
public:
	enum class WrapMode {
		Repeat,
		ClampToEdge
	};
	WrapMode mode;
public:
	Texture2D();
	//Texture2D(const Texture2D&);
	//Texture2D& operator=(const Texture2D&); 
	Texture2D(const Texture2D&) = default;
	Texture2D& operator=(const Texture2D&) = default;
	Texture2D(const char* file);

	// sample function
	glm::vec4 Sample(float x, float y);

};

