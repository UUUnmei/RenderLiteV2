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
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>();
	light->WithPosition(glm::vec3(200.0f, 200.0f, 200.0f))
		.WithDirection(light->target - light->position)
		.WithIntensity(5500.f)
		.WithRange(200.0f);
	light->CalcMVP();
	context->AddLight(light);
	context->EnableShadowMap();

	//context->AddModel("obj/bunny.obj")
	//	.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(250.0f)));
	context->AddModel("obj/mary/Marry.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(25.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(4.0f))
		);
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(4.0f))
		);

	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());


	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender.BindContext(context);
	srender.GetShader().vs.BindMatProj(context->light->projection);
}

void PCSSScene::Draw()
{
	// movable light test
//static float d = 45.0f;
//d += 3.0f;
//context->light->WithPosition(glm::vec3(200 * cosf(glm::radians(d)), 200.0f, 200 * sinf(glm::radians(d))));

	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// shadow pass
	context->SetRenderTarget(context->GetShadowMapPointer());

	for (int i = 0; i < 4; ++i) {
		context->light->BindMatModel(context->models[i]->model_matrix);
		context->light->CalcMVP();
		srender.GetShader().BindLightMVP(context->light->mvp);
		srender.Draw(i);
	}

	context->SetRenderTarget(context->GetFrameBufferPointer());
	context->GetDepthBufferPointer()->Clear();

	//// draw pass
	context->light->BindOwnModel();
	lrender.GetShader().vs.BindMatModel(context->light->model);
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->mesh);  // DrawMesh instead of Draw

	psrender.GetShader().vs.BindMatView(context->camera_view_cache);

	for (int i = 0; i < 4; ++i) {
		psrender.GetShader().vs.BindMatModel(context->models[i]->model_matrix);
		context->light->BindMatModel(context->models[i]->model_matrix);
		context->light->CalcMVP();
		psrender.GetShader().BindLigthMVP(context->light->mvp);
		psrender.Draw(i);
	}

}

void PCSSScene::OnKeyChanged(int key, int scanCode, int action, int mode)
{
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
