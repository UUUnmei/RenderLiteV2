
#include <iostream>
#include "Application.h"
#include "NanosuitScene.h"

int main()
{
    
    Application(std::make_unique<NanosuitScene>(800, 600)).Run();
    

    return 0;
}