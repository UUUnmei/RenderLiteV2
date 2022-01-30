#pragma once

#include "SceneBase.h"
#include "OrbitCamera.h"
#include "PCSS.hpp"
#include "PhongWithShadow.hpp"
#include "LightShader.hpp"
#include "ShadowShader.hpp"
#include "Pipeline.h"

class PCSSScene : public SceneBase
{
	using PCSSRenderer = Pipeline<PCSS>;
    // using PhongShadowRenderer = Pipeline<PhongWithShadow>;
	using LightRenderer = Pipeline<LightShader>;
	using ShadowRenderer = Pipeline<ShadowShader>;
public:
	PCSSRenderer psrender;
    // PhongShadowRenderer psrender;
	LightRenderer lrender;
	ShadowRenderer srender;
	std::shared_ptr<SceneContext> context;

    PCSSScene(uint32_t w, uint32_t h);

    // Í¨¹ý SceneBase ¼Ì³Ð
    OrbitCamera& GetCamera() override;
    const unsigned char* GetFrameBuffer() const override;
    void Init() override;
    void Draw() override;
    void OnKeyChanged(int key, int scanCode, int action, int mode) override;
    void OnMousePositionChanged(double xpos, double ypos) override;
    void OnMouseButtonChanged(int button, int action, int mode) override;
    void OnScrollChanged(double x, double y) override;
};

