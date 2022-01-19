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

	virtual void OnKeyChanged(int key, int scanCode, int action, int mode) override final;

	// Í¨¹ý SceneBase ¼Ì³Ð
	virtual const unsigned char* GetFrameBuffer() const override final;
	virtual OrbitCamera& GetCamera() override;
	virtual void Init() override final;
	virtual void Draw() override final;

};

