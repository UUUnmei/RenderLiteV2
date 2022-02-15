#pragma once

#include "Mesh.h"
#include "LightBase.h"


class PointLight final: public LightBase
{
private:
	float fov{ 3.1415926 * 0.5f };
	float aspect{ 1.0f };
	float size{ 2.0f };
	Mesh mesh; 

public:
	PointLight(const glm::vec3& pos);
	// change the size of the light
	PointLight& WithSize(const float r);

	LightBase& WithPosition(const glm::vec3& pos) override;
	LightBase& WithIntensity(const glm::vec3& i) override;
	LightBase& WithNearZ(const float z) override;
	LightBase& WithFarZ(const float z) override;

public:
	glm::mat4 GetLightMVP(const glm::mat4& model) override;
	glm::vec3 GetDirection(const glm::vec3& pos) override;
	glm::mat4 GetLightModelMatrix(void) override;
	Mesh& GetLightMesh(void) override;

public:
	const float GetLightWorldSize(void) const;
	const float GetLightFrustumSize(void) const;
};


