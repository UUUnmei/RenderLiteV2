#pragma once
#include "../SceneBase.h"
#include "../Pipeline.h"
#include "../PhongWithShadow.hpp"
#include "../LightShader.hpp"
#include "../ShadowShader.hpp"
#include "../DirectionalLight.h"
#include "../OrbitCamera.h"

class SceneHardshadow final: public SceneBase
{
private:
    Pipeline<PhongWithShadow> psrender;
    Pipeline<LightShader> lrender;
    Pipeline<ShadowShader> srender;
    std::shared_ptr<SceneContext> context;

public:
    SceneHardshadow(uint32_t w, uint32_t h);

    virtual ICamera& GetCamera() override;
    virtual const unsigned char* GetFrameBuffer() const override;
    virtual void Init() override;
    virtual void Draw() override;
    virtual void OnKeyChanged(int key, int scanCode, int action, int mode) override;
    virtual void OnMousePositionChanged(double xpos, double ypos) override;
    virtual void OnMouseButtonChanged(int button, int action, int mode) override;
    virtual void OnScrollChanged(double x, double y) override;

};

