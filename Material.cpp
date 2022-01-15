#include "Material.h"

Material::Material()
	: diffuse(nullptr), specular(nullptr)
{
	Ka = Kd = Kd = glm::vec3(-10, -10, -10); // for check validity
}

void Material::Set(aiTextureType type, std::shared_ptr<Texture2D> tex)
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		diffuse = std::make_shared<Texture2D>(*tex);
		break;
	case aiTextureType_SPECULAR:
		specular = std::make_shared<Texture2D>(*tex);
		break;
	default:
		break;
	}
}
