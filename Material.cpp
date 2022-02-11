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
		diffuse = tex;
		break;
	case aiTextureType_SPECULAR:
		specular = tex;
		break;
	case aiTextureType_NORMALS:
		normal = tex;
		break;
	case aiTextureType_HEIGHT:
		normal = tex;
		break;
	default:
		break;
	}
}
