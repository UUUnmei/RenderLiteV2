#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir)
	:direction(glm::normalize(dir)), range(100), mesh(Mesh::GenPlane(10.0f))  
{
	projection = glm::ortho(-range, range, -range, range, 0.1f, 1000.0f); // refer to camera near and far
	WithPosition(pos);
	intensity = glm::vec3(1500.0f);
}

DirectionalLight& DirectionalLight::WithDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
	view = glm::lookAt(position, position - direction, glm::vec3(0.0f, 1.0f, 0.0f));
	return *this;
}

DirectionalLight& DirectionalLight::WithRange(const float r)
{
	assert(r > 0);
	range = r * 0.5f;
	projection = glm::ortho(-range, range, -range, range, 0.1f, 1000.0f);
	vp = projection * view;
	return *this;
}

glm::mat4 DirectionalLight::GetLightMVP(const glm::mat4& model)
{
	return vp * model;
}

glm::vec3 DirectionalLight::GetDirection(const glm::vec3& pos)
{
	return direction; 
}

glm::mat4 DirectionalLight::GetLightModelMatrix(void)
{
	// not very precise
	glm::vec3 lightDir = -direction;
	glm::vec3 oriDir = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 axis = glm::cross(oriDir, lightDir);
	float rad = std::acos(glm::dot(oriDir, lightDir));
	glm::mat4 model = 
		glm::rotate(
			glm::translate(glm::mat4(1.0f), position),
			rad, axis
		);
	return model;
}

Mesh& DirectionalLight::GetLightMesh(void)
{
	return mesh;
}

LightBase& DirectionalLight::WithPosition(const glm::vec3& pos)
{
	position = pos;
	view = glm::lookAt(position, position - direction, glm::vec3(0.0f, 1.0f, 0.0f));
	vp = projection * view;
	return *this;
}

LightBase& DirectionalLight::WithIntensity(const glm::vec3& i)
{
	intensity = i;
	return *this;
}
