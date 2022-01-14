#include "CubeScene.h"
#include "OrbitCamera.h"

CubeScene::CubeScene(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>(w, h))
{
	width = w;
	height = h;
}

const unsigned char* CubeScene::GetFrameBuffer() const
{
	return context->GetFrameBufferPointer().Get();
}

void CubeScene::Init()
{
	std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(width, height, glm::radians(45.0f), width * 1.0f / height, 0.1, 1000.f);
	context->AddCamera( cam );

	context->AddModel("obj/cube.obj")
		.BindModelMat(glm::mat4(5.0f));
	context->AddModel("obj/cube.obj")
		.BindModelTex("obj/pet.png")
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));

	PipelineConfig cfg;
	cfg.draw_mode = ConfigParams::DrawMode::WireFrame;

	nrender.BindContext(context);
	drender.BindContext(context);
	drender.SetConfig(cfg);

}

void CubeScene::Draw()
{

//init 
// 	Add*
//	renderer1.bind(context);
//	renderer2.bind(context);
//	renderer3.bind(context);
//
//update
//  Clear()
// 	renderer1.draw(0);
//	renderer1.draw(1);
//	renderer2.draw(2);
//	renderer3.draw(3);

	context->ClearBuffer();
	nrender.Draw(0);
	drender.Draw(1);

}

OrbitCamera& CubeScene::GetCamera()
{
	return *context->camera;
}
