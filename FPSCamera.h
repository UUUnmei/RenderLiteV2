#pragma once
#include "ICamera.h"

class FPSCamera final : public ICamera
{
private:
	// 和显示窗口相关
	int width;
	int height;
	float fovY_rad;
	float ratio_wh;
	float near_z;
	float far_z;
public:
	FPSCamera(int width, int height, float fovY_rad, float ratio_wh, float near_z, float far_z);
	void SetPosition(float x, float y, float z);
	void SetDirection(float x, float y, float z); //从(x,y,z)看向原点

private:
	float rotate_speed{ 1.0f };
	float move_speed{ 1.0f };
	// update
	bool front;
	bool left;
	bool right;
	bool back;
	bool up;
	bool down;

	glm::vec3 pos;
	glm::vec3 _up{ 0.0f, 1.0f, 0.0f };
	float phi{ 0.0f };
	float theta{ 90.0f };
	void update_phi_theta(float x, float y);
	glm::vec3 cal_direction(void);
	void reset(void);
	bool enable{ true };
public:
	glm::vec3 GetCameraPosition() override;
	glm::mat4 GetView() override;
	glm::mat4 GetProj() override;

	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;
};

