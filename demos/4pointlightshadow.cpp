#include "4pointlightshadow.h"

ScenePointLightShadow::ScenePointLightShadow(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

ICamera& ScenePointLightShadow::GetCamera()
{
	return *context->camera;
}

const unsigned char* ScenePointLightShadow::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer()->Get();
}

void ScenePointLightShadow::Init()
{
	std::shared_ptr<ICamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera(cam);
	std::shared_ptr<PointLight> light = std::make_shared<PointLight>(glm::vec3(0.0f, 50.0f, 50.0f));
	light->WithFarZ(200.0f);
	light->EnableShadowMap(1);
	context->AddLight(light);

	context->AddCube(10.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 15.0f, 0.0f)));
	context->AddCube(10.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 15.0f, 25.0f)));
	context->AddCube(10.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, 15.0f, 25.0f)));
	context->AddCube(10.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 15.0f, 90.0f)));
	context->AddCube(10.0f)
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 70.0f, 60.0f)));

	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 1.0f, 8.0f)));
	context->AddModel("obj/floor/floor.obj")
		.BindModelMat(
			glm::translate(
				glm::rotate(glm::mat4(1.0f), 3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f)),
				glm::vec3(0.0f, -80.0f, 0.0f)
			) *
			glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 1.0f, 8.0f)));

	psrender.BindContext(context);
	psrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	lrender.BindContext(context);
	lrender.GetShader().vs.BindMatProj(context->camera->GetProj());

	srender_point.BindContext(context);
	srender_point.GetShader().vs.BindMatProj(light->GetProj());
}

void ScenePointLightShadow::Draw()
{
	context->ClearBuffer();
	context->camera_pos_cache = context->camera->GetCameraPosition();
	context->camera_view_cache = context->camera->GetView();

	// shadow pass
	for (int j = 0; j < 6; ++j) {
		//context->light->GetShadowMap(j)->Clear(glm::vec4(1.0f));
		context->SetRenderTarget(context->light->GetShadowMap(j));
		for (int i = 0; i < context->models.size(); ++i) {
			srender_point.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix, j);
			srender_point.GetShader().vs.model = context->models[i]->model_matrix; //need world space
			srender_point.Draw(i);
		}
		context->GetDepthBufferPointer()->Clear();
	}
	context->SetRenderTarget(context->GetFrameBufferPointer());

	// draw pass
	lrender.GetShader().vs.BindMatModel(context->light->GetLightModelMatrix());
	lrender.GetShader().vs.BindMatView(context->camera_view_cache);
	lrender.DrawMesh(context->light->GetLightMesh());  // DrawMesh instead of Draw

	psrender.GetShader().vs.BindMatView(context->camera_view_cache);
	for (size_t i = 0; i < context->models.size(); ++i) {
		psrender.GetShader().vs.BindMatModel(context->models[i]->model_matrix);
		// actually point light doesn't need light_mvp here
		psrender.GetShader().vs.light_mvp = context->light->GetLightMVP(context->models[i]->model_matrix, 0);
		psrender.Draw(i);
	}
}

void ScenePointLightShadow::OnKeyChanged(int key, int scanCode, int action, int mode)
{
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		for (int i = 0; i < 6; ++i) {  // not exactly depth value
			std::string name = "depth_" + std::to_string(i) + ".png";
			Image::Dump(name.c_str(),
				context->light->GetShadowMap(i)->GetWidth(),
				context->light->GetShadowMap(i)->GetHeight(), 4,
				context->light->GetShadowMap(i)->Get());
		}
	}
}

void ScenePointLightShadow::OnMousePositionChanged(double xpos, double ypos)
{
}

void ScenePointLightShadow::OnMouseButtonChanged(int button, int action, int mode)
{
}

void ScenePointLightShadow::OnScrollChanged(double x, double y)
{
}
