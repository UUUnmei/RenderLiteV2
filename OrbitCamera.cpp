#include "OrbitCamera.h"

#include <GLFW/glfw3.h>

#include <iostream>

OrbitCamera::OrbitCamera(int width, int height, float fovY_rad, float ratio_wh, float near_z, float far_z)
	:width(width), height(height), fovY_rad(fovY_rad), ratio_wh(ratio_wh), near_z(near_z), far_z(far_z),
	 isLeftMousePressed(false), isRightMousePressed(false), isFirstMouseEvent(true)
{
	UP = glm::vec3(0.0f, 1.0f, 0.0f);
	reset();
}

glm::mat4 OrbitCamera::GetProj() {
	return glm::perspective(fovY_rad, ratio_wh, near_z, far_z);
}

glm::mat4 OrbitCamera::GetView() {
	glm::vec3 pos = GetCameraPosition();
	return glm::lookAt(pos, target, UP);
}

void OrbitCamera::reset() {
	phi = DEFAULT_PHI;
	theta = DEFAULT_THETA;
	distance = DEFAULT_DISTANCE;

	SetTarget();
}

void OrbitCamera::SetTarget(const glm::vec3& tar) {
	target = tar;
}

glm::vec3 OrbitCamera::GetCameraPosition()
{
	float t = glm::radians(theta);
	float p = glm::radians(phi);

	float x = distance * std::sin(t) * std::cos(p);
	float y = distance * std::sin(t) * std::sin(p);
	float z = distance * std::cos(t);
	// ע������ϵ�ı仯
	return glm::vec3(
		target.x + y,
		target.y + z,
		target.z + x
	);
}


void OrbitCamera::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_LEFT_SHIFT)
		reset();
}

void OrbitCamera::OnMousePositionChanged(double xpos, double ypos) 
{

	if (isFirstMouseEvent)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMouseEvent = false;
		return;
	}

	float dx = xpos - lastX;
	float dy = ypos - lastY;

	if (isLeftMousePressed) {		
		update_phi_theta(dx / height, dy / height);
	}

	if (isRightMousePressed) {
		update_target(dx / height, dy / height);
	}
		
	lastX = xpos;
	lastY = ypos;

}

void OrbitCamera::OnMouseButtonChanged(int button, int action, int mode) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		isLeftMousePressed = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		isRightMousePressed = (action == GLFW_PRESS);
	}
}

void OrbitCamera::OnScrollChanged(double x, double y) 
{
	// һ�����xûɶ�ã������Ϲ�һ����λy=1.�¹�һ����λy=-1
	//std::cout << x << ' ' << y << '\n';
	distance *= std::pow(0.95f, y);
}


void OrbitCamera::update_phi_theta(float x, float y)
{

	// xy�Ǻ�����λ���봰�ڸ߶�֮�ȣ�������ֻȡ�߶ȣ�������Ϊ��߱�Ӱ�첻ͬ������ٶȣ�
	float d_theta = y * 180.0f;  // תΪ�Ƕ�
	float d_phi = x * 360.0f;

	phi -= d_phi * rotate_speed;
	theta -= d_theta * rotate_speed;

	//phi = Math::clamp(phi, MIN_PHI, MAX_PHI);		// ����������ת
	if (theta >= MAX_THETA) theta = MAX_THETA;	// ����ת������������һ�£�������¡����Ϸ���
	if (theta <= MIN_THETA) theta = MIN_THETA;

}

void OrbitCamera::update_target(float x, float y)
{
	glm::vec3 cur = GetCameraPosition();
	glm::vec3 forward = glm::normalize(target - cur);
	glm::vec3 left = glm::cross(UP, forward);  
	glm::vec3 up = glm::cross(forward, left);

	float factor = distance * (float)tan(fovY_rad / 2) * 2;
	glm::vec3 dx = x * factor * left;
	glm::vec3 dy = y * factor * up;

	target += dx + dy;

}