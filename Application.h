#pragma once

#include "Window.h"
#include "GLShader.h"
#include "Timer.h"
#include "SceneBase.h"

#include <memory>


class Application
{
private:
	std::unique_ptr<Window> window;
	std::unique_ptr<SceneBase> scene;
	Timer timer;

	// some opengl stuff
	uint32_t vao, vbo, ibo, texture;
	std::unique_ptr<GLShader> glshader;
	void _init_workflow();
	void _update();
	void UpdateFPS();
	void Init();
	void SetCallBacks();
public:
	Application(std::unique_ptr<SceneBase> demo_scene);
	~Application() = default;

	void Run();
	

};

