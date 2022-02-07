#pragma once

/*
* �������
* ��ס�����ק����ת���������ţ���ס�Ҽ��ƶ�Ŀ��㣨���ģ�
* ��shift��λ
*
* ʵ�ֲο� https://github.com/zauonlok/renderer/blob/master/renderer/core/camera.c �Լ�test_helper.c�еĲ�������
* ����ԭ���ϱȽϼ��ԣ��������� ���λ��x ���� ���ڸ� �õ�һ�� 01 ֮������� ������� 2pi ������Ҫת���Ļ��ȡ�����Ч����ʵ������
* ���䣺�������ﴰ�ں����ʵ��ʱ���ڰ�ס������£���ʹ��겻�ڴ��ڷ�Χ��Ҳ���Խ��ܵ���Ϣ���ʳ��Ը�֮��������Դ���1
*
* ���и����ӣ������Ͻ�Trackball���켣�򣿣�������ʵ�������Ч������
* ���ӣ� http://www.songho.ca/opengl/gl_camera.html ����trackball������
* ���������רҵ֮�����ڼ�������Ļ�ϵĵ㵽���Ƶ������ͶӰλ�ã�ʵ�ʼ���������λ��֮�����ת��ͽǶȡ�
*/

/*
ע

�������������õ�����ϵ�����������
   z
   |
   |
	---- y
  /
 /
x
�ⲿ�ּ����ǻ�������������
get_view()ʱҪת��������
ʵ������ռ������ϵ
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
	float phi;  // ��������
	float theta;
	
	glm::vec3 UP;  // ���Ϸ�����ʱд��
	glm::vec3 target; // �������λ�ã�Ҳ�൱�ڻ��Ƶ�����
	float distance;   // ��İ뾶
	
	bool isLeftMousePressed;
	bool isRightMousePressed;
	bool isFirstMouseEvent;
	float lastX;
	float lastY;

	float rotate_speed = 1.0f;	// ��ת�����ŵ��ٶ�����
	float zoom_speed = 1.0f;

	// ����ʾ�������
	int width;
	int height;
	float fovY_rad;
	float ratio_wh;
	float near_z;
	float far_z;

public:
	OrbitCamera(int width, int height, float fovY_rad, float ratio_wh, float near_z, float far_z);
	void SetTarget(const glm::vec3& tar = glm::vec3(0.0f));

	glm::mat4 GetView() override;  // ��ȡview transform����
	glm::mat4 GetProj() override;  // ��ȡ͸�ӱ任����


	// ����ꡢ���̿���
	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;

	
	// �ָ�Ĭ��λ��
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

