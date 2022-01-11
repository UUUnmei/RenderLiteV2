#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> v, const std::vector<uint32_t> i, int material_idx)
	: vertices(v), indices(i), material_idx(material_idx) { }
