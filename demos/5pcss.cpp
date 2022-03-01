#include "5pcss.h"

ScenePCSS::ScenePCSS(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& ScenePCSS::GetCamera()
{
	return *context->camera;
}

const unsigned char* ScenePCSS::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void ScenePCSS::Init()
{
	std::shared_ptr<ICamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<PointLight> light = std::make_shared<PointLight>(glm::vec3(0.0f, 100.0f, 100.0f));
	light->WithNearZ(1.0f).WithFarZ(200.0f).EnableShadowMap(1);
	context->AddLight(light);
						// those parameters matter a lot
	context->AddModel("obj/mary/Marry.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 4.0f)));

	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender.BindContext(context);
	srender.GetShader().vs.BindMatProj(light->GetProj());
}

void ScenePCSS::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// shadow pass
	context->SetRenderTarget(context->light->GetShadowMap(0));
	for (size_t i = 0; i < context->models.size(); ++i) {
		// here use point light as a spot light towards negative z
		srender.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix, 0);
		srender.Draw(i);
	}
	context->SetRenderTarget(context->GetFrameBufferPointer());
	context->GetDepthBufferPointer()->Clear();

	// draw pass
	lrender.GetShader().vs.BindMatModel(context->light->GetLightModelMatrix());
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->GetLightMesh());  // DrawMesh instead of Draw

	psrender.GetShader().vs.BindMatView(context->camera_view_cache);
	for (size_t i = 0; i < context->models.size(); ++i) {
		psrender.GetShader().vs.BindMatModel(context->models[i]->model_matrix);
		psrender.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix, 0);
		psrender.Draw(i);
	}
}

void ScenePCSS::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void ScenePCSS::OnMousePositionChanged(double xpos, double ypos)
{
}

void ScenePCSS::OnMouseButtonChanged(int button, int action, int mode)
{
}

void ScenePCSS::OnScrollChanged(double x, double y)
{
}
