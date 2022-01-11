#pragma once

#include "SceneBase.h"
#include "NormalShader.hpp"
#include "Pipeline.h"

class CubeScene : public SceneBase
{
	using Pipeline = Pipeline<NormalShader>;
public:
	Pipeline pipe;

	CubeScene(uint32_t w, uint32_t h);



	// Í¨¹ý SceneBase ¼Ì³Ð
	virtual const unsigned char* GetFrameBuffer() const override final;

	virtual void Init() override final;

	virtual void Draw() override final;

};

