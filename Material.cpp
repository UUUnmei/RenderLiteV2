#include "Material.h"

Material::Material()
	: diffuse(nullptr)
{

}

void Material::Set(aiTextureType type, std::shared_ptr<Texture2D> tex)
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		diffuse = std::make_shared<Texture2D>(*tex);
		break;
	default:
		break;
	}
}
