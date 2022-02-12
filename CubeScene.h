#pragma once

#include "SceneBase.h"
#include "ICamera.h"
#include "NormalShader.hpp"
#include "SkyboxShader.hpp"
#include "DiffuseShader.hpp"
#include "TangentShader.hpp"
#include "Pipeline.h"
#include "DirectionalLight.h"

class CubeScene final: public SceneBase
{
	using NormalRenderer = Pipeline<NormalShader>;
	using DiffuseRenderer = Pipeline<DiffuseShader>;
public:
	NormalRenderer nrender;
	DiffuseRenderer drender;
	Pipeline<TangentShader> trender;
	Pipeline<SkyboxShader> skyrender;
	std::shared_ptr<SceneContext> context;
	
	CubeScene(uint32_t w, uint32_t h);

	

	// Í¨¹ý SceneBase ¼Ì³Ð
	const unsigned char* GetFrameBuffer() const override;
	ICamera& GetCamera() override;
	void Init() override;
	void Draw() override;

	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;

};

