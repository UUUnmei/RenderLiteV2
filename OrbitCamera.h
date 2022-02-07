#pragma once

/*
* 环绕相机
* 按住左键拖拽可旋转，滚轮缩放，按住右键移动目标点（球心）
* 左shift复位
*
* 实现参考 https://github.com/zauonlok/renderer/blob/master/renderer/core/camera.c 以及test_helper.c中的部分内容
* 这里原理上比较简略，大致上是 鼠标位移x 除以 窗口高 得到一个 01 之间的数， 将其乘以 2pi 视作需要转动的弧度。不过效果其实还可以
* 补充：由于这里窗口和鼠标实现时，在按住的情况下，即使鼠标不在窗口范围内也可以接受到消息，故除以高之后的数可以大于1
*
* 还有个例子，名义上叫Trackball（轨迹球？），不过实际上最后效果类似
* 链接： http://www.songho.ca/opengl/gl_camera.html 下面trackball的例子
* 不过这个的专业之处在于计算了屏幕上的点到环绕的球面的投影位置，实际计算了两个位置之间的旋转轴和角度。
*/

/*
注

这里球面坐标用的坐标系（方便起见）
   z
   |
   |
	---- y
  /
 /
x
这部分计算是基于上面的坐标的
get_view()时要转换到下面
实际相机空间的坐标系
   y
   |
   |
	---- x
  /
 /
z

*/

#include "ICamera.h"

class OrbitCamera final : public ICamera
{

private:
	float phi;  // 球面坐标
	float theta;
	
	glm::vec3 UP;  // 向上方向暂时写死
	glm::vec3 target; // 相机看的位置，也相当于环绕的球心
	float distance;   // 球的半径
	
	bool isLeftMousePressed;
	bool isRightMousePressed;
	bool isFirstMouseEvent;
	float lastX;
	float lastY;

	float rotate_speed = 1.0f;	// 旋转和缩放的速度因子
	float zoom_speed = 1.0f;

	// 和显示窗口相关
	int width;
	int height;
	float fovY_rad;
	float ratio_wh;
	float near_z;
	float far_z;

public:
	OrbitCamera(int width, int height, float fovY_rad, float ratio_wh, float near_z, float far_z);
	void SetTarget(const glm::vec3& tar = glm::vec3(0.0f));

	glm::mat4 GetView() override;  // 获取view transform矩阵
	glm::mat4 GetProj() override;  // 获取透视变换矩阵


	// 由鼠标、键盘控制
	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;

	
	// 恢复默认位置
	void reset(void);

	glm::vec3 GetCameraPosition() override;

private:
	void update_phi_theta(float x, float y);
	void update_target(float x, float y);

	static constexpr float DEFAULT_PHI = 0.0f;
	static constexpr float DEFAULT_THETA = 90.0f;
	static constexpr float DEFAULT_DISTANCE = 10.0f;
	static constexpr float MIN_PHI = -179.0f;
	static constexpr float MAX_PHI = 179.0f;
	static constexpr float MIN_THETA = 1.0f;
	static constexpr float MAX_THETA = 179.0f;
	static constexpr float MIN_DISTANCE = 0;
	static constexpr float MAX_DISTANCE = 1e4;
};

