#include "3hardshadow.h"

SceneHardshadow::SceneHardshadow(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& SceneHardshadow::GetCamera()
{
	return *context->camera;
}

const unsigned char* SceneHardshadow::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void SceneHardshadow::Init()
{
	std::shared_ptr<ICamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(glm::vec3(50.0f), glm::vec3(1.0f));
	light->WithRange(200.0f).WithFarZ(200.0f).EnableShadowMap(1);
	context->AddLight(light);

	context->AddModel("obj/mary/Marry.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));

	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender.BindContext(context);
	srender.GetShader().vs.BindMatProj(light->GetProj());

}

void SceneHardshadow::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// shadow pass
	context->SetRenderTarget(context->light->GetShadowMap(0));
	for (size_t i = 0; i < context->models.size(); ++i) {
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

void SceneHardshadow::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		std::string name = "depth.png"; // not exactly depth value
		Image::Dump(name.c_str(),
			context->light->GetShadowMap(0)->GetWidth(),
			context->light->GetShadowMap(0)->GetHeight(), 
			4,
			context->light->GetShadowMap(0)->Get());
	}
}

void SceneHardshadow::OnMousePositionChanged(double xpos, double ypos)
{
}

void SceneHardshadow::OnMouseButtonChanged(int button, int action, int mode)
{
}

void SceneHardshadow::OnScrollChanged(double x, double y)
{
}
