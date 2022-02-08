
#include <iostream>
#include "Application.h"
#include "PCSSScene.h"
#include "CubeScene.h"


int main()
{
    
    //Application(std::make_unique<PCSSScene>(800, 600)).Run();
    Application(std::make_unique<CubeScene>(800, 600)).Run();

    return 0;
}