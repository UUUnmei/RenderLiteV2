#include "PCSSScene.h"
#include "OrbitCamera.h"
PCSSScene::PCSSScene(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& PCSSScene::GetCamera()
{
	return *context->camera;
}

const unsigned char* PCSSScene::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void PCSSScene::Init()
{
	std::shared_ptr<ICamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(glm::vec3(50.0f), glm::vec3(1.0f));
	light->WithRange(200.0f).WithFarZ(200.0f);
	context->AddLight(light);
	context->EnableShadowMap(1);

	//context->AddModel("obj/bunny.obj")
	//	.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(250.0f)));
	context->AddModel("obj/mary/Marry.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
	//context->AddModel("obj/nanosuit/nanosuit.obj")
	//	.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(5.0f)));
	/*context->AddUVSphere(1.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f))
			, glm::vec3(0.0f, 2.0f, 0.0f)));*/
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));

	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender.BindContext(context);
	srender.GetShader().vs.BindMatProj(light->GetProj());
}

void PCSSScene::Draw()
{

	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// shadow pass
	context->SetRenderTarget(context->GetShadowMapPointer());

	for (int i = 0; i < 2; ++i) {
		srender.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix);
		srender.Draw(i);
	}

	context->SetRenderTarget(context->GetFrameBufferPointer());
	context->GetDepthBufferPointer()->Clear();

	// draw pass
	lrender.GetShader().vs.BindMatModel(context->light->GetLightModelMatrix());
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->GetLightMesh());  // DrawMesh instead of Draw

	psrender.GetShader().vs.BindMatView(context->camera_view_cache);
	for (int i = 0; i < 2; ++i) {
		psrender.GetShader().vs.BindMatModel(context->models[i]->model_matrix);
		psrender.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix);
		psrender.Draw(i);
	}

}

void PCSSScene::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_S) {
		Image::Dump("depth.png", context->GetShadowMapPointer()->GetWidth(), context->GetShadowMapPointer()->GetHeight(), 4, context->GetShadowMapPointer()->Get());
	}
}

void PCSSScene::OnMousePositionChanged(double xpos, double ypos)
{
}

void PCSSScene::OnMouseButtonChanged(int button, int action, int mode)
{
}

void PCSSScene::OnScrollChanged(double x, double y)
{
}
