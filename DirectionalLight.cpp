#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
	:mesh(Mesh::GenPlane(1.0f))
{
}

DirectionalLight& DirectionalLight::WithPosition(const glm::vec3& pos)
{
	position = pos;
	direction = glm::normalize(target - pos);
	view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	vp = projection * view;
	return *this;
}

DirectionalLight& DirectionalLight::WithDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
	return *this;
}

DirectionalLight& DirectionalLight::WithIntensity(const float i)
{
	intensity = glm::vec3(i);
	return *this;
}

DirectionalLight& DirectionalLight::WithRange(const float r)
{
	mesh = Mesh::GenPlane(r);
	range = r * 0.5f;
	projection = glm::ortho(-range, range, -range, range, 0.1f, 1000.0f);
	vp = projection * view;
	return *this;
}

void DirectionalLight::BindOwnModel(void)  //bind light's model matrix
{
	glm::vec3 lightDir = direction;
	glm::vec3 oriDir = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 axis = glm::cross(oriDir, lightDir);
	float rad = std::acos(glm::dot(oriDir, lightDir));
	model = 
		glm::rotate(
			glm::translate(glm::mat4(1.0f), position),
			rad, axis
		);
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




