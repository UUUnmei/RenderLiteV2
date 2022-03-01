#include "1blinnphong.h"

SceneBlinnPhong::SceneBlinnPhong(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& SceneBlinnPhong::GetCamera()
{
	return *context->camera;
}

const unsigned char* SceneBlinnPhong::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void SceneBlinnPhong::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(glm::vec3(20.0f), glm::vec3(1.0f));
	light->WithRange(200.0f).WithFarZ(200.0f);
	context->AddLight(light);
	
	context->AddModel("obj/nanosuit/nanosuit.obj");

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	prender.BindContext(context);
	prender.GetShader().vs.BindMatProj(context->camera->GetProj());

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void SceneBlinnPhong::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	lrender.GetShader().vs.BindMatModel(context->light->GetLightModelMatrix());
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->GetLightMesh());  // DrawMesh instead of Draw

	prender.GetShader().vs.BindMatView(context->camera_view_cache);
	prender.Draw(0);
}

void SceneBlinnPhong::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void SceneBlinnPhong::OnMousePositionChanged(double xpos, double ypos)
{
}

void SceneBlinnPhong::OnMouseButtonChanged(int button, int action, int mode)
{
}

void SceneBlinnPhong::OnScrollChanged(double x, double y)
{
}
