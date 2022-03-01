#pragma once

#include "../SceneBase.h"
#include "../OrbitCamera.h"
#include "../PointLight.h"
#include "../Pipeline.h"
#include "../PhongWithShadow.hpp"
#include "../LightShader.hpp"
#include "../PointLightShadowShader.hpp"


class ScenePointLightShadow final: public SceneBase
{
private:
    Pipeline<PhongWithShadow> psrender;
    Pipeline<LightShader> lrender;
    Pipeline<PointLightShadowShader> srender_point;
    std::shared_ptr<SceneContext> context; 
public:
    ScenePointLightShadow(uint32_t w, uint32_t h);

    virtual ICamera& GetCamera() override;
    virtual const unsigned char* GetFrameBuffer() const override;
    virtual void Init() override;
    virtual void Draw() override;
    virtual void OnKeyChanged(int key, int scanCode, int action, int mode) override;
    virtual void OnMousePositionChanged(double xpos, double ypos) override;
    virtual void OnMouseButtonChanged(int button, int action, int mode) override;
    virtual void OnScrollChanged(double x, double y) override;

};

