
#include <iostream>
#include "Application.h"
#include "PCSSScene.h"

int main()
{
    
    Application(std::make_unique<PCSSScene>(800, 600)).Run();

    return 0;
}