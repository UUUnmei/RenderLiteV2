#include "FPSCamera.h"

#include <GLFW/glfw3.h>

FPSCamera::FPSCamera(int width, int height, float fovY_rad, float ratio_wh, float near_z, float far_z)
    :width(width), height(height), fovY_rad(fovY_rad), ratio_wh(ratio_wh), near_z(near_z), far_z(far_z),
    front(false), left(false), right(false), back(false), up(false), down(false)
{
	SetPosition(10, 10, 10);
	SetDirection(1, 1, 1);
}

void FPSCamera::SetPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void FPSCamera::SetDirection(float x, float y, float z)
{
	float d = std::sqrtf(x * x + y * y + z * z);
	assert(d > 0.0f);
	d = 1.0f / d;
	x *= d, y *= d, z *= d;

	theta = std::acosf(z) * 180.0f / 3.1415926;
	phi = std::atan2f(y, x) * 180.0f / 3.1415926; 
}

void FPSCamera::update_phi_theta(float x, float y)
{
	// xy是横纵向位移与窗口高度之比，（这里只取高度，避免因为宽高比影响不同方向的速度）
	float d_theta = y * 180.0f;  // 转为角度
	float d_phi = x * 360.0f;

	phi -= d_phi * rotate_speed;
	theta -= d_theta * rotate_speed;

	if (theta >= 179.0f) theta = 179.0f;	// 上下转（俯仰）限制一下，避免更新“向上方向”
	if (theta <= 1.0f) theta = 1.0f;
}

glm::vec3 FPSCamera::cal_direction(void)
{
	float t = glm::radians(theta);
	float p = glm::radians(phi);

	float x = std::sin(t) * std::cos(p);
	float y = std::sin(t) * std::sin(p);
	float z = std::cos(t);

	return glm::vec3(y, z, x);
}

void FPSCamera::reset(void)
{
	SetPosition(10, 10, 10);
	SetDirection(1, 1, 1);
	phi = 0.0f;
	theta = 90.0f;
}

glm::vec3 FPSCamera::GetCameraPosition()
{
	return pos;
}

glm::mat4 FPSCamera::GetView()
{
	static float last = glfwGetTime();
	float now = glfwGetTime();
	move_speed = 200 * (now - last);
	rotate_speed = 10 * (now - last);
	last = now;
	return glm::lookAt(pos, pos - cal_direction(), _up);
}

glm::mat4 FPSCamera::GetProj()
{
    return glm::perspective(fovY_rad, ratio_wh, near_z, far_z);
}	

#define UPDATE(mov, scan) \
	if (key == scan) {   \
		if (action == GLFW_PRESS)	\ 
			mov = true;		\
		else if (action == GLFW_RELEASE)	\
			mov = false;			\
	}			\

void FPSCamera::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_LEFT_ALT)
		if (action == GLFW_PRESS)
			enable = !enable;
	if (!enable) return;

	if (key == GLFW_KEY_LEFT_SHIFT)
		reset();

	UPDATE(front, GLFW_KEY_W)
	UPDATE(back, GLFW_KEY_S)
	UPDATE(left, GLFW_KEY_A)
	UPDATE(right, GLFW_KEY_D)
	UPDATE(up, GLFW_KEY_SPACE)
	UPDATE(down, GLFW_KEY_LEFT_CONTROL)

	glm::vec3 dir = -cal_direction();
	glm::vec3 _front = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
	glm::vec3 _left = glm::normalize(glm::cross(_up, _front));

	pos += _front * (float)(front - back) * move_speed;
	pos += _left * (float)(left - right) * move_speed;
	pos += _up * (float)(up - down) * move_speed;
}

void FPSCamera::OnMousePositionChanged(double xpos, double ypos)
{
	if (!enable) return;

	static float lastX = width / 2.0;
	static float lastY = height / 2.0;
	
	float dx = xpos - lastX;
	float dy = ypos - lastY;

	update_phi_theta(dx / height, dy / height);
	
	lastX = xpos;
	lastY = ypos;
}

void FPSCamera::OnMouseButtonChanged(int button, int action, int mode)
{
}

void FPSCamera::OnScrollChanged(double x, double y)
{
}
