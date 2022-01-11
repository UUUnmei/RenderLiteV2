#pragma once
#include "SceneContext.h"
#include <memory>
class SceneBase
{
public:
	std::shared_ptr<SceneContext> context;
	uint32_t width;
	uint32_t height;
public:
	SceneBase(uint32_t w, uint32_t h);

	virtual Model& AddModel(const char* path);
	virtual void AddCamera(std::shared_ptr<OrbitCamera> camera);
	virtual OrbitCamera& GetCamera();
	virtual const unsigned char* GetFrameBuffer() const = 0;
	virtual void Init() = 0;
	virtual void Draw() = 0;
	
};

