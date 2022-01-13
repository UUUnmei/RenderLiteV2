#pragma once
#include "OrbitCamera.h"

class SceneBase
{
	
public:
	uint32_t width;
	uint32_t height;

	virtual OrbitCamera& GetCamera() = 0;
	virtual const unsigned char* GetFrameBuffer() const = 0;
	virtual void Init() = 0;
	virtual void Draw() = 0;
	
};

