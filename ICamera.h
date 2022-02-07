#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ICamera {
public:
	virtual glm::vec3 GetCameraPosition() = 0;
	virtual glm::mat4 GetView() = 0; 
	virtual glm::mat4 GetProj() = 0;

	virtual void OnKeyChanged(int key, int scanCode, int action, int mode) = 0;
	virtual void OnMousePositionChanged(double xpos, double ypos) = 0;
	virtual void OnMouseButtonChanged(int button, int action, int mode) = 0;
	virtual void OnScrollChanged(double x, double y) = 0;

	virtual ~ICamera() = default;
};