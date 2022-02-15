#pragma once

#include "FrameBuffer.h"
#include "Mesh.h"
#include "LightBase.h"

#include <memory>

class DirectionalLight final: public LightBase
{
private:
	glm::vec3 direction;
	float range;
	Mesh mesh;

public:
	// here dir is treated as a vector from origin to (dir.x, dir.y, dir.z)
	DirectionalLight(const glm::vec3& pos, const glm::vec3& dir);
	// here dir is treated as a vector from origin to (dir.x, dir.y, dir.z)
	DirectionalLight& WithDirection(const glm::vec3& dir);
	// change the size of area covered by the light
	// not light itself
	DirectionalLight& WithRange(const float r);

	LightBase& WithPosition(const glm::vec3& pos) override;
	LightBase& WithIntensity(const glm::vec3& pos) override;
	LightBase& WithNearZ(const float z) override;
	LightBase& WithFarZ(const float z) override;
public:

	glm::mat4 GetLightMVP(const glm::mat4& model) override;
	glm::vec3 GetDirection(const glm::vec3& pos) override;
	glm::mat4 GetLightModelMatrix(void) override;
	Mesh& GetLightMesh(void) override;
};

