#pragma once

#include "Vertex.h"
#include <vector>
#include <string>

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	int material_idx; 
public:
	Mesh(const std::vector<Vertex> v, const std::vector<uint32_t> i, int material_idx);
};

