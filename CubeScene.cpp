#include "CubeScene.h"
#include "OrbitCamera.h"
#include "FPSCamera.h"

CubeScene::CubeScene(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

const unsigned char* CubeScene::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void CubeScene::Init()
{
	//std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 10000.f);
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

void CubeScene::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// draw pass
	drender.GetShader().vs.BindMatView(context->camera_view_cache);
	drender.Draw(0);
}

void CubeScene::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void CubeScene::OnMousePositionChanged(double xpos, double ypos)
{
}

void CubeScene::OnMouseButtonChanged(int button, int action, int mode)
{
}

void CubeScene::OnScrollChanged(double x, double y)
{
}

ICamera& CubeScene::GetCamera()
{
	return *context->camera;
}
