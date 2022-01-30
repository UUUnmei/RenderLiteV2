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
	return context->GetFrameBufferPointer()->Get();
}

void CubeScene::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera( cam );
	
	//context->AddCube(3.0f);
	context->AddModel("obj/bunny.obj");

	//PipelineConfig cfg;
	//cfg.draw_mode = ConfigParams::DrawMode::WireFrame;
	//cfg.fc_order = ConfigParams::FaceCullOrder::NONE;
	//nrender.BindConfig(cfg);
	
	nrender.BindContext(context);
	nrender.GetShader().vs.BindMatProj(context->camera->GetProj());
	nrender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void CubeScene::Draw()
{
	
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();
	
	// draw pass
	nrender.GetShader().vs.BindMatView(context->camera_view_cache);
	nrender.Draw(0);

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

OrbitCamera& CubeScene::GetCamera()
{
	return *context->camera;
}
