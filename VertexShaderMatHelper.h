#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class VertexShaderMatHelper
{
public:
	glm::mat4 model{ 1.0f };
	glm::mat4 view{ 1.0f };
	glm::mat4 proj{ 1.0f };
	glm::mat4 proj_view{ 1.0f };
	glm::mat4 obj_to_view_normal{ 1.0f };  // it_view_model
	glm::mat4 obj_to_world_normal{ 1.0f }; // UnityObjectToWorldNormal

public:
	void BindMatModel(const glm::mat4& mat);
	void BindMatView(const glm::mat4& mat);
	void BindMatProj(const glm::mat4& mat);
	void BindAllMat(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p);
};

