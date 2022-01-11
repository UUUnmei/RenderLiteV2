#include "Window.h"
#include <iostream>


void Window::ExecGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	auto wnd = static_cast<Window*>(glfwGetWindowUserPointer(window));
	for (auto& callback : wnd->onKeyCallBacks)
		callback(key, scancode, action, mode);
}

void Window::ExecGlfwCursorPositionCallback(GLFWwindow* window, double x, double y)
{
	auto wnd = static_cast<Window*>(glfwGetWindowUserPointer(window));
	for (auto& callback : wnd->onCursorPosCallBacks)
		callback(x, y);
}

void Window::ExecGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mode)
{
	auto wnd = static_cast<Window*>(glfwGetWindowUserPointer(window));
	for (auto& callback : wnd->onMouseButtonCallBacks)
		callback(button, action, mode);
}

void Window::ExecGlfwScrollCallback(GLFWwindow* window, double x, double y)
{
	auto wnd = static_cast<Window*>(glfwGetWindowUserPointer(window));
	for (auto& callback : wnd->onScrollCallBacks)
		callback(x, y);
}

void Window::AddOnKeyChanged(std::function<void(int key, int scancode, int action, int mode)> callback)
{
	onKeyCallBacks.emplace_back(callback);
}

void Window::AddOnMousePositionChanged(std::function<void(double x, double y)> callback)
{
	onCursorPosCallBacks.emplace_back(callback);
}

void Window::AddOnMouseButtonChanged(std::function<void(int button, int action, int mode)> callback)
{
	onMouseButtonCallBacks.emplace_back(callback);
}

void Window::AddOnScrollChanged(std::function<void(double x, double y)> callback)
{
	onScrollCallBacks.emplace_back(callback);
}

Window::Window(const char* title, int width, int height)
	: title(title), width(width), height(height)
{
	if (!glfwInit())
	{
		std::cout << "glfw init failed\n";
		exit(1);
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);  // 不允许改变窗口大小，所以也没设置相关的回调
	
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		std::cout << "glfwCreateWindow failed\n";
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);	// 关掉垂直同步
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "gladLoadGLLoader failed\n";
		exit(1);
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, ExecGlfwKeyCallback);
	glfwSetCursorPosCallback(window, ExecGlfwCursorPositionCallback);
	glfwSetMouseButtonCallback(window, ExecGlfwMouseButtonCallback);
	glfwSetScrollCallback(window, ExecGlfwScrollCallback);

	glViewport(0, 0, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* Window::Get()
{
	return window;
}
