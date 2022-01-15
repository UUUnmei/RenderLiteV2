#pragma once 

#include "Model.h"
#include "OrbitCamera.h"
#include "FrameBuffer.h"
#include "DepthBuffer.h"

#include <memory>
#include <vector>

class SceneContext
{

	
public: 
	std::shared_ptr<OrbitCamera> camera;
	glm::vec3 camera_pos_cache; // inorder to call GetCameraPosition "only once every frame" instead of "every (shaded)pixel of every frame"
	std::vector<std::unique_ptr<Model>> models;

public:
	SceneContext(uint32_t w, uint32_t h);
	Model& AddModel(const std::string& path);
	Model& AddPlane(float scale = 1.0f);
	Model& AddCube(float scale = 1.0f);
	Model& AddIcoSphere(float scale, int div = 4); //Icosphere
	Model& AddUVSphere(float scale, int lattDiv = 12, int longDiv = 24); //uvsphere
	void AddCamera(std::shared_ptr<OrbitCamera> camera);

private:
	std::unique_ptr<DepthBuffer> depthbuffer;
	std::unique_ptr<FrameBuffer> framebuffer;
public:
	void ClearBuffer(const glm::vec4& bg = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	FrameBuffer& GetFrameBufferPointer() noexcept;
	DepthBuffer& GetDepthBufferPointer() noexcept;
};

