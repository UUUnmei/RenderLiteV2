#pragma once

#include "FrameBuffer.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class DirectionalLight
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 intensity;
	float range;
	glm::vec3 target{ 0.0f };

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;  // ortho
	glm::mat4 vp; 
	glm::mat4 mvp;

	Mesh mesh;

public:
	DirectionalLight();
	DirectionalLight& WithPosition(const glm::vec3& pos);
	DirectionalLight& WithDirection(const glm::vec3& dir);
	DirectionalLight& WithIntensity(const float i);
	DirectionalLight& WithRange(const float r);
	void BindOwnModel(void);

public:
	void BindMatModel(const glm::mat4& mat);
	void BindMatView(const glm::mat4& mat);
	void BindMatProj(const glm::mat4& mat);
	void CalcMVP(void);

};

