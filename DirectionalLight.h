#pragma once

#include "FrameBuffer.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <memory>

class DirectionalLight
{
public:
	glm::vec3 position;
	glm::vec3 intensity;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;  // ortho
	glm::mat4 vp; 
	glm::mat4 mvp;

	Mesh mesh;

public:
	DirectionalLight(const glm::vec3& position, const glm::vec3& intensity);
	void BindMatModel(const glm::mat4& mat);
	void BindMatView(const glm::mat4& mat);
	void BindMatProj(const glm::mat4& mat);
	void CalcMVP(void);

};

