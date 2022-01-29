#pragma once

#include <cstdint>

class OrbitCamera;

class SceneBase
{
	
public:
	uint32_t width;
	uint32_t height;

	virtual ~SceneBase() = default;
	virtual OrbitCamera& GetCamera() = 0;
	virtual const unsigned char* GetFrameBuffer() const = 0;
	virtual void Init() = 0;
	virtual void Draw() = 0;
	
	virtual void OnKeyChanged(int key, int scanCode, int action, int mode) = 0;
	virtual void OnMousePositionChanged(double xpos, double ypos) = 0;
	virtual void OnMouseButtonChanged(int button, int action, int mode) = 0;
	virtual void OnScrollChanged(double x, double y) = 0;
};

