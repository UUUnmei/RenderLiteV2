
#include <iostream>
#include "Application.h"
#include "CubeScene.h"


int main()
{
    
    Application(std::make_unique<CubeScene>(800, 600)).Run();

    return 0;
}