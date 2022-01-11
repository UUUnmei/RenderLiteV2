#include "SceneContext.h"

SceneContext::SceneContext()
	: camera(nullptr)
{
}

Model& SceneContext::AddModel(const std::string& path)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->LoadFromFile(path);
	models.push_back(std::move( m ));
	return *(models.back().get());
}

void SceneContext::AddCamera(std::shared_ptr<OrbitCamera> cam)
{
	camera = cam;
}
