#include "DirectionalLight.h"
#include "SceneContext.h"
#include "ShadowShader.hpp"
#include "Pipeline.h"

DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir)
	:direction(glm::normalize(dir)), range(100), mesh(Mesh::GenPlane(10.0f)),
	shadow_map{nullptr}
{
	projection = glm::ortho(-range, range, -range, range, nearz, farz); 
	WithPosition(pos);
	intensity = glm::vec3(1500.0f);
}

DirectionalLight& DirectionalLight::WithDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
	view = glm::lookAt(position, position - direction, glm::vec3(0.0f, 1.0f, 0.0f));
	vp = projection * view;
	return *this;
}

DirectionalLight& DirectionalLight::WithRange(const float r)
{
	assert(r > 0);
	range = r * 0.5f;
	projection = glm::ortho(-range, range, -range, range, nearz, farz);
	vp = projection * view;
	return *this;
}

glm::mat4 DirectionalLight::GetLightMVP(const glm::mat4& model, int slot)
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

FrameBuffer* DirectionalLight::GetShadowMap(int slot)
{
	assert(shadow_map);
	return shadow_map.get();
}

// here shadowCoord is ndc space
float DirectionalLight::LookUpShadowMap(const glm::vec4& shadowCoord)
{
	float x = shadowCoord.x * shadow_map->GetWidth();
	float y = shadowCoord.y * shadow_map->GetHeight();
	glm::vec4 vZ;
	shadow_map->read(x, y, vZ);
	float fZ = DecodeFloatFromRGBA(vZ);
	return fZ;
}

void DirectionalLight::EnableShadowMap(int scale)
{
	assert(scale == 1 || scale == 2);
	shadow_map = std::make_unique<FrameBuffer>(1024 * scale, 1024 * scale);
}

LightBase& DirectionalLight::WithNearZ(const float z)
{
	nearz = z;
	projection = glm::ortho(-range, range, -range, range, nearz, farz);
	vp = projection * view;
	return *this;
}

LightBase& DirectionalLight::WithFarZ(const float z)
{
	farz = z;
	projection = glm::ortho(-range, range, -range, range, nearz, farz);
	vp = projection * view;
	return *this;
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
