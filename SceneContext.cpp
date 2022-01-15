#include "SceneContext.h"

SceneContext::SceneContext(uint32_t w, uint32_t h)
	: camera(nullptr)
{
	framebuffer = std::make_unique<FrameBuffer>(w, h);
	depthbuffer = std::make_unique<DepthBuffer>(w, h);
}

Model& SceneContext::AddModel(const std::string& path)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->LoadFromFile(path);
	models.push_back(std::move( m ));
	return *(models.back().get());
}

Model& SceneContext::AddPlane(float scale)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenPlane(scale));
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddCube(float scale)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenCube(scale));
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddIcoSphere(float scale, int div)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenSphere(scale, div));
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddUVSphere(float scale, int lattDiv, int longDiv)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenSphere(scale, lattDiv, longDiv));
	models.push_back(std::move(m));
	return *(models.back().get());
}

void SceneContext::AddCamera(std::shared_ptr<OrbitCamera> cam)
{
	camera = cam;
}

void SceneContext::ClearBuffer(const glm::vec4& bg)
{
	framebuffer->Clear(bg);
	depthbuffer->Clear();
}

FrameBuffer& SceneContext::GetFrameBufferPointer() noexcept
{
	return *framebuffer;
}

DepthBuffer& SceneContext::GetDepthBufferPointer() noexcept
{
	return *depthbuffer;
}
