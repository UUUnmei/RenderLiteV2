//reference: https://github.com/Zielon/CPURasterizer/blob/main/Rasterizer/Rasterizer/src/Viewer
// 不想用原生win32写窗口了，借用下opengl，绘制时实际上是把framebuffer绘制到纹理上，见application
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <vector>

class Window
{
private:
	// 窗口的基本属性
	GLFWwindow* window;
	std::string title;
	int width;
	int height;
	// 存放回调函数
	std::vector<std::function<void(double x, double y)>> onCursorPosCallBacks;
	std::vector<std::function<void(int button, int action, int mode)>> onMouseButtonCallBacks;
	std::vector<std::function<void(int key, int scancode, int action, int mode)>> onKeyCallBacks;
	std::vector<std::function<void(double x, double y)>> onScrollCallBacks;
	// 处理回调函数
	static void ExecGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void ExecGlfwCursorPositionCallback(GLFWwindow* window, double x, double y);
	static void ExecGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
	static void ExecGlfwScrollCallback(GLFWwindow* window, double x, double y);

public:
	// 添加回调函数
	void AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mode)> callback);
	void AddOnMousePositionChanged(std::function<void(double x, double y)> callback);
	void AddOnMouseButtonChanged(std::function<void(int button, int action, int mode)> callback);
	void AddOnScrollChanged(std::function<void(double x, double y)> callback);

	// 基操
	Window(const char* title, int width, int height);
	~Window();

	GLFWwindow* Get();


};

