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
	
	std::shared_ptr<Skybox> sky = std::make_shared<Skybox>("D:\\Visual Studio\\RenderLite\\obj\\skybox\\");
	context->AddSkybox( sky );

	context->AddCube(2.0f);
	//context->AddModel("obj/bunny.obj"); 
	//context->AddPlane(100.0f)
	//	.BindModelMat(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
	//	.BindModelTex("obj/uvchecker/1/4096x4096 Texel Density Texture 1.png");
	//context->AddModel("obj/Sponza/sponza.obj");
	
	nrender.BindContext(context);
	nrender.GetShader().vs.BindMatProj(context->camera->GetProj());
	nrender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);

	PipelineConfig cfg;
	cfg.fc_order = ConfigParams::FaceCullOrder::CW;
	skyrender.BindConfig(cfg);
	skyrender.BindContext(context);
	skyrender.GetShader().vs.BindMatProj(context->camera->GetProj());

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

	skyrender.GetShader().vs.BindMatView(context->camera_view_cache);
	skyrender.DrawMesh(context->skybox->mesh);

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
