#include "2skybox.h"

SceneSkybox::SceneSkybox(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& SceneSkybox::GetCamera()
{
	return *context->camera;
}

const unsigned char* SceneSkybox::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void SceneSkybox::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<Skybox> sky = std::make_shared<Skybox>("obj/skybox/default/");
	context->AddSkybox(sky);

	context->AddCube(30.0f).BindModelTex("obj/miptest.jpeg");
	//context->AddCube(1.0f);

	//nrender.BindContext(context);
	//nrender.GetShader().vs.BindMatProj(context->camera->GetProj());
	drender.BindContext(context);
	drender.GetShader().vs.BindMatProj(context->camera->GetProj());

	PipelineConfig cfg;
	cfg.fc_order = ConfigParams::FaceCullOrder::CW;
	skyrender.BindConfig(cfg);
	skyrender.BindContext(context);
	skyrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void SceneSkybox::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();
	
	//nrender.GetShader().vs.BindMatView(context->camera_view_cache);
	//nrender.Draw(0);
	drender.GetShader().vs.BindMatView(context->camera_view_cache);
	drender.Draw(0);

	skyrender.GetShader().vs.BindMatView(context->camera_view_cache);
	//skyrender.DrawMesh(context->skybox->mesh);
}

void SceneSkybox::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void SceneSkybox::OnMousePositionChanged(double xpos, double ypos)
{
}

void SceneSkybox::OnMouseButtonChanged(int button, int action, int mode)
{
}

void SceneSkybox::OnScrollChanged(double x, double y)
{
}
