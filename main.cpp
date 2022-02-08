
#include <iostream>
#include "Application.h"
#include "PCSSScene.h"
#include "CubeScene.h"



int main()
{
    
    //Application(std::make_unique<PCSSScene>(800, 600)).Run();
    //Application(std::make_unique<CubeScene>(800, 600)).Run();

    Texture2D tex("obj/miptest.jpeg");
    tex.EnableMipmap();
    for (int i = 0; i < tex.mips.size(); ++i) {
        auto& p = tex.mips[i];
        Image::Dump((std::to_string(i) + ".png").c_str(), 
            p->GetWidth(), p->GetHeight(), p->GetBpp(), p->Get());
    }


    return 0;
}