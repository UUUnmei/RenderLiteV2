#pragma once

#include "Mesh.h"
#include "LightBase.h"
#include "FrameBuffer.h"

#include <memory>

/*
* for now point light shadow(omnidirectional shadow) only impl in PhongWithShadow.hpp(hard shadow)
* not in PCSS !
* but point light can still work with PCSS(and hardshadow) as a spot light 
* when GetLightMVP, can use one of 6 existing vps, or change it manually for a moment
*/
class PointLight final: public LightBase
{
private:
	float fov{ 3.1415926 * 0.5f };
	float aspect{ 1.0f };
	float size{ 2.0f };
	Mesh mesh; 

	const glm::vec3 directions[6]{
		{0.0f, 0.0f, 1.0f},//front
		{0.0f, 0.0f, -1.0f},//back
		{0.0f, 1.0f, 0.0f},//bottom
		{0.0f, -1.0f, 0.0f},//top
		{1.0f, 0.0f, 0.0f},//left
		{-1.0f, 0.0f, 0.0f},//right
	};
	const glm::vec3 ups[6]{
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
	};
	glm::mat4 vps[6];
	float invH;
	float invW;
	void UpdateVPs(void);
	
public:

	PointLight(const glm::vec3& pos);
	// change the size of the light
	PointLight& WithSize(const float r);

	LightBase& WithPosition(const glm::vec3& pos) override;
	LightBase& WithIntensity(const glm::vec3& i) override;
	LightBase& WithNearZ(const float z) override;
	LightBase& WithFarZ(const float z) override;

public:
	glm::vec3 GetDirection(const glm::vec3& pos) override;
	glm::mat4 GetLightModelMatrix(void) override;
	glm::mat4 GetLightMVP(const glm::mat4& model, int slot) override;
	Mesh& GetLightMesh(void) override;

public:
	const float GetLightWorldSize(void) const;
	const float GetLightFrustumSize(void) const;

private:
	std::unique_ptr<FrameBuffer> shadow_map[6];
public:
	void EnableShadowMap(int scale) override;
	float LookUpShadowMap(const glm::vec4& shadowCoord) override;
	FrameBuffer* GetShadowMap(int slot) override;
};


