#include "SceneContext.h"

SceneContext::SceneContext(uint32_t w, uint32_t h)
	: skybox(nullptr), light(nullptr), camera(nullptr)
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
	m->ComputeTangent();
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddCube(float scale)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenCube(scale));
	m->ComputeTangent();
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddIcoSphere(float scale, int div)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenSphere(scale, div));
	m->ComputeTangent();
	models.push_back(std::move(m));
	return *(models.back().get());
}

Model& SceneContext::AddUVSphere(float scale, int lattDiv, int longDiv)
{
	std::unique_ptr<Model> m = std::make_unique<Model>();
	m->meshes.push_back(Mesh::GenSphere(scale, lattDiv, longDiv));
	m->ComputeTangent();
	models.push_back(std::move(m));
	return *(models.back().get());
}

void SceneContext::AddCamera(std::shared_ptr<ICamera> cam)
{
	camera = cam;
}

void SceneContext::AddSkybox(std::shared_ptr<Skybox> sky)
{
	skybox = sky;
}

void SceneContext::AddLight(std::shared_ptr<LightBase> L)
{
	light = L;
}

void SceneContext::ClearBuffer(const glm::vec4& bg)
{
	framebuffer->Clear(bg);
	depthbuffer->Clear();
}

FrameBuffer* SceneContext::GetFrameBufferPointer() noexcept
{
	assert(framebuffer != nullptr);
	return framebuffer.get();
}

DepthBuffer* SceneContext::GetDepthBufferPointer() noexcept
{
	assert(depthbuffer != nullptr);
	return depthbuffer.get();
}

FrameBuffer* SceneContext::GetRenderTarget() noexcept
{
	assert(render_target != nullptr);
	return render_target;
}

void SceneContext::SetRenderTarget(FrameBuffer* target)
{
	render_target = target;
/*
* suppose this function will be called in render loop as a switch frequently
* so here choose to maintain the biggest one instead of reallocate every time
*/
	if(depthbuffer->GetWidth() < target->GetWidth()
	|| depthbuffer->GetHeight() < target->GetHeight()) {
		depthbuffer = std::make_unique<DepthBuffer>(target->GetWidth(), target->GetHeight());
	}
}
