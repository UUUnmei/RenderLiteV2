#include "Application.h"
#include "OrbitCamera.h"


#include <iostream>
#include <cstdio>

void Application::_init_workflow()
{
	static float vertices[] =
	{
		1.f, 1.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		1.f, -1.f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.f, -1.f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1.f, 1.f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};

	static unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glshader->Attach("assets/shader/basic.vert");
	glshader->Attach("assets/shader/basic.frag");
	glshader->Link();
	glshader->Bind();
}

void Application::_update()
{
	// Copy frameBuffer to texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		scene->GetFrameBuffer());

	glBindTexture(GL_TEXTURE_2D, texture);

	glshader->Bind();
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Application::UpdateFPS()
{
	static int frames = 0;
	frames++;
	float tm = timer.Peek();

	if (tm > 1.0f) {
		printf("FPS: %.2f\n", frames / tm);
		timer.Mark();
		frames = 0;
	}
}

void Application::Init()
{
	_init_workflow();
	scene->Init();
	SetCallBacks();
	
}

Application::Application(std::unique_ptr<SceneBase> demo_scene)
{
	scene = std::move( demo_scene );
	window = std::make_unique<Window>("TEST", scene->width, scene->height);
	glshader = std::make_unique<GLShader>();  // glfw之类的初始化都在window那边，这里把shader搞成指针，推迟构造到window之后，不然报错
}

void Application::Run()
{
	
	Init();

	while (!glfwWindowShouldClose(window->Get()))
	{

		// update framebuffer
		
		scene->Draw();

		_update();

		glfwSwapBuffers(window->Get());
		glfwPollEvents();

		UpdateFPS();
	}
}

void Application::SetCallBacks()
{

	window->AddOnMousePositionChanged([this](double x, double y) {
		//std::cout << "current mouse pos: " << x << ' ' << y << '\n';
		scene->GetCamera().OnMousePositionChanged(x, y);
		scene->OnMousePositionChanged(x, y);
		});
	window->AddOnKeyChanged([this](int key, int scancode, int action, int mode) {
		scene->OnKeyChanged(key, scancode, action, mode);
		scene->GetCamera().OnKeyChanged(key, scancode, action, mode);
		});
	window->AddOnMouseButtonChanged([this](int button, int action, int mode) {
		scene->GetCamera().OnMouseButtonChanged(button, action, mode);
		scene->OnMouseButtonChanged(button, action, mode);
		});
	window->AddOnScrollChanged([this](double x, double y) {
		scene->GetCamera().OnScrollChanged(x, y);
		scene->OnScrollChanged(x, y);
		});
}
