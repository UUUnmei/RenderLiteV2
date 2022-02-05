#include "VertexShaderMatHelper.h"

void VertexShaderMatHelper::BindMatModel(const glm::mat4& mat)
{
	model = mat;
	obj_to_view_normal = glm::transpose(glm::inverse(view * model));
	obj_to_world_normal = glm::transpose(glm::inverse(model));
}

void VertexShaderMatHelper::BindMatView(const glm::mat4& mat)
{
	view = mat;
	proj_view = proj * view;
	obj_to_view_normal = glm::transpose(glm::inverse(view * model));
}

void VertexShaderMatHelper::BindMatProj(const glm::mat4& mat)
{
	proj = mat;
	proj_view = proj * view;
}

void VertexShaderMatHelper::BindAllMat(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p)
{
	model = m;
	view = v;
	proj = p;
	proj_view = proj * view;
	obj_to_view_normal = glm::transpose(glm::inverse(view * model));
	obj_to_world_normal = glm::transpose(glm::inverse(model));
}

