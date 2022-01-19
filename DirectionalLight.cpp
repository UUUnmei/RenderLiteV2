#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::vec3& intensity)
	:position(position), intensity(intensity), mesh(Mesh::GenCube(0.5f))
{
}

void DirectionalLight::BindMatModel(const glm::mat4& mat)
{
	model = mat;
}

void DirectionalLight::BindMatView(const glm::mat4& mat)
{
	view = mat;
	vp = projection * view;
}

void DirectionalLight::BindMatProj(const glm::mat4& mat)
{
	projection = mat;
	vp = projection * view;
}

void DirectionalLight::CalcMVP(void)
{
	mvp = vp * model;
}




