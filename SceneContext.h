#pragma once 

#include "Model.h"
#include "ICamera.h"
#include "LightBase.h"
#include "FrameBuffer.h"
#include "DepthBuffer.h"
#include "Skybox.h"

#include <memory>
#include <vector>

class SceneContext
{

	
public:
	std::shared_ptr<Skybox> skybox;
	std::shared_ptr<LightBase> light;  // for now only consider one light
	std::shared_ptr<ICamera> camera;
	std::vector<std::unique_ptr<Model>> models;

public:
	// cache for each frame
	glm::vec3 camera_pos_cache; 
	glm::mat4 camera_view_cache; 

public:
	SceneContext(uint32_t w, uint32_t h);
	Model& AddModel(const std::string& path);
	Model& AddPlane(float scale = 1.0f);
	Model& AddCube(float scale = 1.0f);
	Model& AddIcoSphere(float scale, int div = 4); //Icosphere
	Model& AddUVSphere(float scale, int lattDiv = 12, int longDiv = 24); //uvsphere
	void AddCamera(std::shared_ptr<ICamera> cam);
	void AddSkybox(std::shared_ptr<Skybox> sky);
	void AddLight(std::shared_ptr<LightBase> l);

private:
	std::unique_ptr<DepthBuffer> depthbuffer;
	std::unique_ptr<FrameBuffer> framebuffer;
	FrameBuffer* render_target{ nullptr };
public:
	void ClearBuffer(const glm::vec4& bg = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	FrameBuffer* GetFrameBufferPointer() noexcept;
	DepthBuffer* GetDepthBufferPointer() noexcept;
	FrameBuffer* GetRenderTarget() noexcept;
	void SetRenderTarget(FrameBuffer* target);
};

