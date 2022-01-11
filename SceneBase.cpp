#include "SceneBase.h"

SceneBase::SceneBase(uint32_t w, uint32_t h)
	: context(std::make_shared<SceneContext>()), width(w), height(h)
{ }

Model& SceneBase::AddModel(const char* path)
{
	return context->AddModel(path);
}

void SceneBase::AddCamera(std::shared_ptr<OrbitCamera> camera)
{
	context->AddCamera(camera);
}

OrbitCamera& SceneBase::GetCamera()
{
	return *context->camera;
}
