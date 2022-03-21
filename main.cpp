
#include "Application.h"

#include "demos/1blinnphong.h"
#include "demos/2skybox.h"
#include "demos/3hardshadow.h"
#include "demos/4pointlightshadow.h"
#include "demos/5pcss.h"
#include "demos/6normalmapping.h"
#include "demos/7mipmap.h"


int main()
{
    
    //Application(std::make_unique<SceneBlinnPhong>(800, 600)).Run();
    //Application(std::make_unique<SceneSkybox>(800, 600)).Run();
    //Application(std::make_unique<SceneHardshadow>(800, 600)).Run();
    //Application(std::make_unique<ScenePointLightShadow>(800, 600)).Run();
    //Application(std::make_unique<ScenePCSS>(800, 600)).Run();
    //Application(std::make_unique<SceneNormalMapping>(800, 600)).Run();
    Application(std::make_unique<SceneMipmap>(800, 600)).Run();

    return 0;
}