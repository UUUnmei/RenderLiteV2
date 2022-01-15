#pragma once

#include "Texture2D.h"

#include <assimp/material.h>
#include <glm/glm.hpp>

#include <memory>

enum class ColorType {
	AMBIENT,
	DIFFUSE,
	SPECULAR
};

class Material
{
public:
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;

public:
	std::shared_ptr<Texture2D> diffuse;
	std::shared_ptr<Texture2D> specular;


public:
	Material();

	void Set(aiTextureType type, std::shared_ptr<Texture2D> tex);

};

