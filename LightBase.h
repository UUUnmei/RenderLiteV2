#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Mesh;
template<class T> class Pipeline;
class ShadowShader;
class SceneContext;
class FrameBuffer;

class LightBase {
protected:
	glm::vec3 position;
	glm::vec3 intensity;
	float nearz{ 0.1f };
	float farz{ 100.0f };
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
	glm::mat4 vp{ 1.0f };

	// combine with Encode in shadow shader
	float DecodeFloatFromRGBA(const glm::vec4& rgba){
		return glm::dot(rgba, glm::vec4(1.0f, 1 / 255.0f, 1 / 65025.0f, 1 / 16581375.0f));
	}

public:
	// for vs, do face cull
	const glm::mat4& GetProj(void) const { return projection; }
	
	virtual ~LightBase() = default;

	// getter setter
	virtual const glm::vec3& GetPosition(void) const { return position; }
	virtual LightBase& WithPosition(const glm::vec3& pos) = 0;

	virtual const glm::vec3& GetIntensity(void) const { return intensity; }
	virtual LightBase& WithIntensity(const glm::vec3& i) = 0;
	
	virtual const float GetNearZ(void) const { return nearz; }
	virtual LightBase& WithNearZ(const float z) = 0;

	virtual const float GetFarZ(void) const { return farz; }
	// considering the float/shadow precision, farz had better not be too large
	virtual LightBase& WithFarZ(const float z) = 0;


	// 1. get mvp matrix for transform in the light view
	// generally view and projection is constant
	// while model matrix may change, depends on specific model
	// 2. considering point light has 6 directions which means 6 vp matrix
	// can use SLOT to specify one, from 0 to 5
	// directional light ignore that parameter
	//(but still have to give one in terms of default parameter doesn't work well with virtual function)
	virtual glm::mat4 GetLightMVP(const glm::mat4& model, int slot) = 0;
	// give light direction in WORLD SPACE, from given point to the light 
	// pos may be unnecessary (like a directional light)
	virtual glm::vec3 GetDirection(const glm::vec3& pos) = 0;  

	// for visualize light from camera view
	// HAVE TO add a mesh component
	virtual glm::mat4 GetLightModelMatrix(void) = 0;  
	// for visualize 
	// for point light, the mesh also describe its size somehow
	virtual Mesh& GetLightMesh(void) = 0;  

	// move shadow map to light from SceneContext
	virtual FrameBuffer* GetShadowMap(int slot) = 0;
	virtual void EnableShadowMap(int scale) = 0;
	virtual float LookUpShadowMap(const glm::vec4& shadowCoord) = 0;
};
