#pragma once

#include "Mesh.h"
#include "Texture2D.h"

#include <vector>

class Skybox
{
	std::vector<Texture2D> faces;
public:
	Mesh mesh;
	

public:
	// skybox textures DIRECTORY in which should contain 6 textures
	// end with '/' or '\\'
	Skybox(const char* dir);
	glm::vec4 Sample(float x, float y, float z);

	
};

