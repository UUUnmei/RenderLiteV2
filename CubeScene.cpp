#include "CubeScene.h"
#include "OrbitCamera.h"

CubeScene::CubeScene(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

void CubeScene::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_SPACE) {
		Image::Dump("depth.png", 800, 600, 4, context->GetShadowMapPointer()->Get());
		std::cout << "DEPTH output\n";
	}
}

const unsigned char* CubeScene::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void CubeScene::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera( cam );
	std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(glm::vec3(25.0f, 50.0f, 25.0f), glm::vec3(1500.f));
	light->BindMatModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	light->BindMatView(glm::lookAt(light->position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	float e = 100.0f;
	light->BindMatProj(glm::ortho(-e, e, -e, e, 0.1f, 1000.f));
	//light->BindMatProj(glm::perspective(glm::radians(45.0f), width * 1.0f / height, 0.1f, 1000.f));
	light->CalcMVP();
	context->AddLight( light );
	context->EnableShadowMap();

	context->AddModel("obj/nanosuit/nanosuit.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f)));
	//context->AddModel("obj/mary/Marry.obj")
	//	.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(5.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	
	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender.BindContext(context);


	context->SetRenderTarget(context->GetFrameBufferPointer());
}

void CubeScene::Draw()
{
	// movable light test
	static float d = 45.0f;
	d += 5.0f;
	context->light->position = glm::vec3(25 * cosf(glm::radians(d)), 50.0f, 25 * sinf(glm::radians(d)));
	context->light->BindMatView(glm::lookAt(context->light->position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();
	
	// shadow pass
	context->SetRenderTarget(context->GetShadowMapPointer());

	context->light->BindMatModel(context->models[0]->model_matrix);
	context->light->CalcMVP();
	srender.GetShader().BindLightMVP(context->light->mvp);
	srender.Draw(0);

	context->light->BindMatModel(context->models[1]->model_matrix);
	context->light->CalcMVP();
	srender.GetShader().BindLightMVP(context->light->mvp);
	srender.Draw(1);

	context->SetRenderTarget(context->GetFrameBufferPointer());
	context->GetDepthBufferPointer()->Clear();

	// draw pass
	lrender.GetShader().vs.BindMatModel(glm::translate(glm::mat4(1.0f), context->light->position));
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->mesh);  // DrawMesh instead of Draw

	psrender.GetShader().vs.BindMatView(context->camera_view_cache);

	psrender.GetShader().vs.BindMatModel(context->models[0]->model_matrix);
	context->light->BindMatModel(context->models[0]->model_matrix);
	context->light->CalcMVP();
	psrender.GetShader().BindLigthMVP(context->light->mvp);
	psrender.Draw(0);
	

	psrender.GetShader().vs.BindMatModel(context->models[1]->model_matrix);
	context->light->BindMatModel(context->models[1]->model_matrix);
	context->light->CalcMVP();
	psrender.GetShader().BindLigthMVP(context->light->mvp);
	psrender.Draw(1);
	
}

OrbitCamera& CubeScene::GetCamera()
{
	return *context->camera;
}
