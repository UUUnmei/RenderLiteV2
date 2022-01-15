#include "CubeScene.h"
#include "OrbitCamera.h"

CubeScene::CubeScene(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

const unsigned char* CubeScene::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer().Get();
}

void CubeScene::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera( cam );

	context->AddModel("obj/nanosuit/nanosuit.obj");


	PipelineConfig cfg;
	cfg.draw_mode = ConfigParams::DrawMode::WireFrame;

	//nrender.BindContext(context);
	//drender.BindContext(context);
	prender.BindContext(context);
	//nrender.SetConfig(cfg);

}

void CubeScene::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	prender.Draw(0);
	
	
}

OrbitCamera& CubeScene::GetCamera()
{
	return *context->camera;
}
