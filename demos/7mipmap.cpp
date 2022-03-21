#include "7mipmap.h"
#include "../FPSCamera.h"

SceneMipmap::SceneMipmap(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

const unsigned char* SceneMipmap::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void SceneMipmap::Init()
{
	std::shared_ptr<FPSCamera> cam = std::make_shared<FPSCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 10000.f);
	cam->SetPosition(5, 5, 5);
	cam->SetDirection(1, 1, 1);
	context->AddCamera( cam );
	
	context->AddModel("obj/Sponza/sponza.obj");
	
	drender.BindContext(context);
	drender.GetShader().vs.BindMatProj(context->camera->GetProj());
	drender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void SceneMipmap::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// draw pass
	drender.GetShader().vs.BindMatView(context->camera_view_cache);
	drender.Draw(0);
}

void SceneMipmap::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void SceneMipmap::OnMousePositionChanged(double xpos, double ypos)
{
}

void SceneMipmap::OnMouseButtonChanged(int button, int action, int mode)
{
}

void SceneMipmap::OnScrollChanged(double x, double y)
{
}

ICamera& SceneMipmap::GetCamera()
{
	return *context->camera;
}
