#include "Material.h"

Material::Material()
	: diffuse(nullptr), specular(nullptr)
{
	Ka = Ks = glm::vec3(0.0f); 
	Kd = glm::vec3(0.6f);
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
	case aiTextureType_DISPLACEMENT:
		disp = tex;
		break;
	default:
		break;
	}
}
