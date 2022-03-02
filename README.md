# RenderLiteV2

cpu rasterizer 
with a more powerful architecture(than [version 1](https://github.com/UUUnmei/RenderLite))
implement some basic technique

## Features
- programmable pipeline
- perspective correct
- directional light
- point light (with **omnidirectional shadow!**)
- changable render target (render to window or texture, enable a depth pass for shadow)
- basic geometry generation (plane, cube, uvsphere, icosphere)
- face culling 
- cvv clip
- homogeneous clip
- orbit camera
- fps camera
- tangent space calculation
- mouse/keyboard input callbacks (thanks to glfw)
- mipmap (but not used in practice because of partial derivative)

## Some implemented technique
> mainly in *Shader.hpp
- basic shadow mapping(hard shadow)
- PCSS
- **omnidirectional shadow** for point light
- skybox
- normal mapping, parallax papping
- blinn-phong shading
- an experimental partial derivative calculating(e.g. ddx/ddy in HLSL) for mipmap (see test_mipmap branch for detail，a little different from GPU result)

## TODO
- [ ] update pipeline (greatly inspired by [this](https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/) and related series)
- [ ] refactor texture and material (for flexibility)
- [ ] tiled based (for performance and try multi-thread)

## Dependencies
- Windows (though windows and input system is based on glfw, i haven't test on other platform)
- C++17
- stb_image v2.27 (included already)
- stb_image_write v1.16 (included already)
- assimp-5.1.5
- glm-0.9.9.8
- glfw 3.34
- [glad](https://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gl%3D4.6)
- git lfs (due to large model files)

> Visual Studio seems not work with git-lfs well, will generate extra 'hook' and 'lfs' directory. 
What's worse, may need to replace .git/lfs/object by this extra lfs/object when you update files tracked by lfs

## Reference
- GAMES101
- fundamentals-of-computer-graphics-4th
- LearnOpenGL (for some technique)
- [planetchili/3D_Fundamentals](https://github.com/planetchili/3D_Fundamentals) (for pipeline, shader design) 
- [Zielon/CPURasterizer](https://github.com/Zielon/CPURasterizer/tree/main/Rasterizer/Rasterizer/src/Viewer) (for glfw windows usage)
- and many others in comments

## Example
> see demos

basic blinn phong
![](https://s2.loli.net/2022/03/02/KnuG7S2hdjsrTe8.png)

skybox
![](https://s2.loli.net/2022/03/02/gxtOuHBRpZKjvkd.png)

hard shadow
![](https://s2.loli.net/2022/02/15/t7PuVdlyX3K96Zn.png)

**omnidirectional shadow** for point light
![](https://s2.loli.net/2022/02/28/idrgsD7qNEPj2no.png)
![](https://s2.loli.net/2022/02/15/9SzDcmxCGkQVqJU.gif)

PCSS
![](https://s2.loli.net/2022/03/02/fNFGpotszSlWabC.png)

normal mapping
![](https://s2.loli.net/2022/03/02/k39UlDeo1j6msz5.png)
![](https://s2.loli.net/2022/03/02/TEzCu4jke8gIso1.png)
