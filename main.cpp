
#include "Application.h"
#include "demos/6normalmapping.h"


int main()
{
    
    Application(std::make_unique<SceneNormalMapping>(800, 600)).Run();

    return 0;
}