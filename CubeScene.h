#pragma once

#include "SceneBase.h"
#include "OrbitCamera.h"
#include "NormalShader.hpp"
#include "DiffuseShader.hpp"
#include "PhongShader.hpp"
#include "PhongWithShadow.hpp"
#include "LightShader.hpp"
#include "ShadowShader.hpp"
#include "Pipeline.h"

class CubeScene final: public SceneBase
{
	using NormalRenderer = Pipeline<NormalShader>;
	using DiffuseRenderer = Pipeline<DiffuseShader>;
	using PhongRenderer = Pipeline<PhongShader>;
	using PhongShadowRenderer = Pipeline<PhongWithShadow>;
	using LightRenderer = Pipeline<LightShader>;
	using ShadowRenderer = Pipeline<ShadowShader>;
public:
	NormalRenderer nrender;
	DiffuseRenderer drender;
	PhongRenderer prender;
	PhongShadowRenderer psrender;
	LightRenderer lrender;
	ShadowRenderer srender;
	std::shared_ptr<SceneContext> context;
	
	CubeScene(uint32_t w, uint32_t h);

	

	// Í¨¹ý SceneBase ¼Ì³Ð
	const unsigned char* GetFrameBuffer() const override;
	OrbitCamera& GetCamera() override;
	void Init() override;
	void Draw() override;

	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;

};

