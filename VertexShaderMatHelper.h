#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class VertexShaderMatHelper
{
public:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);;
	glm::mat4 proj = glm::mat4(1.0f);;
	glm::mat4 proj_view = glm::mat4(1.0f);;
	glm::mat4 it_view_model = glm::mat4(1.0f);;

public:
	void BindMatModel(const glm::mat4& mat);
	void BindMatView(const glm::mat4& mat);
	void BindMatProj(const glm::mat4& mat);
};

