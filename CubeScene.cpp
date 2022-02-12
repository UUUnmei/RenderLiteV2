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
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	//std::shared_ptr<FPSCamera> cam = std::make_shared<FPSCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 10000.f);
	//cam->SetPosition(5, 5, 5);
	//cam->SetDirection(1, 1, 1);
	context->AddCamera( cam );
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>();
	light->WithPosition(glm::vec3(50.0f, 50.0f, 50.0f))
		.WithDirection(light->target - light->position)
		.WithIntensity(2500.f)
		.WithRange(200.0f);
	light->CalcMVP();
	context->AddLight(light);
	//std::shared_ptr<Skybox> sky = std::make_shared<Skybox>("D:\\Visual Studio\\RenderLiteV2\\obj\\skybox\\default\\");
	//context->AddSkybox( sky );

	//context->AddCube(4.0f).BindModelTex("obj/normal.jpg");
	context->AddModel("obj/toybox/floor.obj");
		//.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f)));
		
	//context->AddPlane(100.0f)
	//	.BindModelMat(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))
	//	.BindModelTex("obj/normal.jpg");
	//context->AddModel("obj/Sponza/sponza.obj");
	
	trender.BindContext(context);
	trender.GetShader().vs.BindMatProj(context->camera->GetProj());
	trender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);
	trender.GetShader().vs.pContext = context;

	PipelineConfig cfg;
	cfg.fc_order = ConfigParams::FaceCullOrder::CW;
	//skyrender.BindConfig(cfg);
	//skyrender.BindContext(context);
	//skyrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void CubeScene::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();
static float d = 45.0f;
d += 3.0f;
context->light->WithPosition(glm::vec3(50 * cosf(glm::radians(d)), 50.0f, 50 * sinf(glm::radians(d))));


	// draw pass
	trender.GetShader().vs.BindMatView(context->camera_view_cache);
	trender.Draw(0);

	//skyrender.GetShader().vs.BindMatView(context->camera_view_cache);
	//skyrender.DrawMesh(context->skybox->mesh);

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
