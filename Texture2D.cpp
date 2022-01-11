#include "Texture2D.h"

Texture2D::Texture2D()
	: texture(nullptr)
{ }

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
{ }

glm::vec4 Texture2D::Sample(float x, float y)
{
	glm::vec4 color(0, 0, 0, 1);

	// GL_REPEAT
	x -= std::floor(x);
	y -= std::floor(y);
	
	texture->read(x * texture->GetWidth() + 0.5f, (1 - y) * texture->GetHeight() + 0.5f, color);
	return color;
}
