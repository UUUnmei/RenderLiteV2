#include "VertexShaderMatHelper.h"

void VertexShaderMatHelper::BindMatModel(const glm::mat4& mat)
{
	model = mat;
	it_view_model = glm::transpose(glm::inverse(view * model));
}

void VertexShaderMatHelper::BindMatView(const glm::mat4& mat)
{
	view = mat;
	proj_view = proj * view;
	it_view_model = glm::transpose(glm::inverse(view * model));
}

void VertexShaderMatHelper::BindMatProj(const glm::mat4& mat)
{
	proj = mat;
	proj_view = proj * view;
}
