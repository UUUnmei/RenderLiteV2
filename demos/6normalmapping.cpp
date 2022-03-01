#include "6normalmapping.h"

SceneNormalMapping::SceneNormalMapping(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& SceneNormalMapping::GetCamera()
{
	return *context->camera;
}

const unsigned char* SceneNormalMapping::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void SceneNormalMapping::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(glm::vec3(50.0f), glm::vec3(1.0f));
	light->WithRange(200.0f).WithFarZ(200.0f);
	context->AddLight(light);

	context->AddModel("obj/nanosuit/nanosuit.obj"); // something went wrong at the head of this model
	//context->AddModel("obj/toybox/floor.obj")
	//	.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f)));

	trender.BindContext(context);
	trender.GetShader().vs.BindMatProj(context->camera->GetProj());
	trender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);
	trender.GetShader().vs.pContext = context;

	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void SceneNormalMapping::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

//manually move the light
	static float d = 45.0f;
	d += 3.0f;
	context->light->WithPosition(glm::vec3(50 * cosf(glm::radians(d)), 50.0f, 50 * sinf(glm::radians(d))));

	// draw pass
	trender.GetShader().vs.BindMatView(context->camera_view_cache);
	trender.Draw(0);
}

void SceneNormalMapping::OnKeyChanged(int key, int scanCode, int action, int mode)
{
}

void SceneNormalMapping::OnMousePositionChanged(double xpos, double ypos)
{
}

void SceneNormalMapping::OnMouseButtonChanged(int button, int action, int mode)
{
}

void SceneNormalMapping::OnScrollChanged(double x, double y)
{
}
