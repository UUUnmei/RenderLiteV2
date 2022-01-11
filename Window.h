//reference: https://github.com/Zielon/CPURasterizer/blob/main/Rasterizer/Rasterizer/src/Viewer
// ������ԭ��win32д�����ˣ�������opengl������ʱʵ�����ǰ�framebuffer���Ƶ������ϣ���application
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <vector>

class Window
{
private:
	// ���ڵĻ�������
	GLFWwindow* window;
	std::string title;
	int width;
	int height;
	// ��Żص�����
	std::vector<std::function<void(double x, double y)>> onCursorPosCallBacks;
	std::vector<std::function<void(int button, int action, int mode)>> onMouseButtonCallBacks;
	std::vector<std::function<void(int key, int scancode, int action, int mode)>> onKeyCallBacks;
	std::vector<std::function<void(double x, double y)>> onScrollCallBacks;
	// ����ص�����
	static void ExecGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void ExecGlfwCursorPositionCallback(GLFWwindow* window, double x, double y);
	static void ExecGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
	static void ExecGlfwScrollCallback(GLFWwindow* window, double x, double y);

public:
	// ��ӻص�����
	void AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mode)> callback);
	void AddOnMousePositionChanged(std::function<void(double x, double y)> callback);
	void AddOnMouseButtonChanged(std::function<void(int button, int action, int mode)> callback);
	void AddOnScrollChanged(std::function<void(double x, double y)> callback);

	// ����
	Window(const char* title, int width, int height);
	~Window();

	GLFWwindow* Get();


};

