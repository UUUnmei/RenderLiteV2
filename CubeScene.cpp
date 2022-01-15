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

	//context->AddUVSphere(3.0f);

	context->AddIcoSphere(3.0f, 0)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f)));
	context->AddIcoSphere(3.0f, 1)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, 0.0f)));
	context->AddIcoSphere(3.0f, 2)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 0.0f, 0.0f)));
	context->AddIcoSphere(3.0f, 3)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, -6.0f, 0.0f)));
	context->AddIcoSphere(3.0f, 4)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, -6.0f, 0.0f)));
	context->AddIcoSphere(3.0f, 5)
		.BindModelMat(glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, -6.0f, 0.0f)));

	PipelineConfig cfg;
	cfg.draw_mode = ConfigParams::DrawMode::WireFrame;

	nrender.BindContext(context);
	//drender.BindContext(context);
	nrender.SetConfig(cfg);

}

void CubeScene::Draw()
{
	context->ClearBuffer();

	for (int i = 0; i < 6; ++i) {
		nrender.Draw(i);
	}
	
	
}

OrbitCamera& CubeScene::GetCamera()
{
	return *context->camera;
}
