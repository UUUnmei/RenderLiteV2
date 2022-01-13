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
	std::vector<std::unique_ptr<Model>> models;

public:
	SceneContext(uint32_t w, uint32_t h);
	Model& AddModel(const std::string& path);
	void AddCamera(std::shared_ptr<OrbitCamera> camera);

private:
	std::unique_ptr<DepthBuffer> depthbuffer;
	std::unique_ptr<FrameBuffer> framebuffer;
public:
	void ClearBuffer(const glm::vec4& bg = glm::vec4(0.2f, 0.2f, 0.8f, 1.0f));
	FrameBuffer& GetFrameBufferPointer() noexcept;
	DepthBuffer& GetDepthBufferPointer() noexcept;
};

