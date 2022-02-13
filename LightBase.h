#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Mesh;

class LightBase {
protected:
	glm::vec3 position;
	glm::vec3 intensity;
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
	glm::mat4 vp{ 1.0f };

public:
	// for vs, do face cull
	const glm::mat4& GetProj(void) const { return projection; }
	
	virtual ~LightBase() = default;

	// getter setter
	virtual const glm::vec3& GetPosition(void) const { return position; }
	virtual LightBase& WithPosition(const glm::vec3& pos) = 0;

	virtual const glm::vec3& GetIntensity(void) const { return intensity; }
	virtual LightBase& WithIntensity(const glm::vec3& i) = 0;
	

	// get mvp matrix for transform in the light view
	// generally view and projection is constant
	// while model matrix may change, depends on specific model
	virtual glm::mat4 GetLightMVP(const glm::mat4& model) = 0;
	// give light direction in WORLD SPACE, from given point to the light 
	// pos may be unnecessary (like a directional light)
	virtual glm::vec3 GetDirection(const glm::vec3& pos) = 0;  

	// for visualize light from camera view
	// HAVE TO add a mesh component
	virtual glm::mat4 GetLightModelMatrix(void) = 0;  
	// for visualize 
	// for point light, the mesh also describe its size somehow
	virtual Mesh& GetLightMesh(void) = 0;  
};