#pragma once

#include <glm/glm.hpp>
struct Vertex {
	glm::vec3 position{ 0.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texcoord{ 0.0f };

	glm::vec3 tangent{ 0.0f };
	glm::vec3 bitangent{ 0.0f };
};
