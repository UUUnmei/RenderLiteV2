#include "NanosuitScene.h"

NanosuitScene::NanosuitScene(uint32_t w, uint32_t h)
    : SceneBase(w, h), pipe(w, h)
{
}

const unsigned char* NanosuitScene::GetFrameBuffer() const
{
    return pipe.GetRawFrameBufferPointer();
}

void NanosuitScene::Init()
{
    std::shared_ptr<OrbitCamera> cam = std::make_shared<OrbitCamera>(800, 600, glm::radians(45.0f), 800.0 / 600.0, 0.1, 1000.f);
    AddCamera(cam);

    AddModel("obj/nanosuit/nanosuit.obj");
}

void NanosuitScene::Draw()
{
	pipe.BeginFrame();
	pipe.BindContext(context);
	for (size_t i = 0; i < context->models.size(); ++i) {
		const auto& model = context->models[i];
		pipe.BindModelMat(model->model_matrix);
		for (size_t j = 0; j < model->meshes.size(); ++j) {
			pipe.DrawMesh(model->meshes[j], i, j);
		}
	}
}
