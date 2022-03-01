#pragma once

#include "../SceneBase.h"
#include "../OrbitCamera.h"
#include "../PointLight.h"
#include "../Pipeline.h"
#include "../LightShader.hpp"
#include "../ShadowShader.hpp"
#include "../PCSS.hpp"


class ScenePCSS final: public SceneBase
{
private:
    Pipeline<PCSS> psrender;
    Pipeline<LightShader> lrender;
    Pipeline<ShadowShader> srender;
    std::shared_ptr<SceneContext> context;
public:
    ScenePCSS(uint32_t w, uint32_t h);

    virtual ICamera& GetCamera() override;
    virtual const unsigned char* GetFrameBuffer() const override;
    virtual void Init() override;
    virtual void Draw() override;
    virtual void OnKeyChanged(int key, int scanCode, int action, int mode) override;
    virtual void OnMousePositionChanged(double xpos, double ypos) override;
    virtual void OnMouseButtonChanged(int button, int action, int mode) override;
    virtual void OnScrollChanged(double x, double y) override;
};

