#pragma once

#include "Texture2D.h"

#include <assimp/material.h>

#include <memory>

class Material
{
public:

	std::shared_ptr<Texture2D> diffuse;


public:
	Material();

	void Set(aiTextureType type, std::shared_ptr<Texture2D> tex);

};

