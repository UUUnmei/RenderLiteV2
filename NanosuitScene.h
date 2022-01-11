#pragma once

#include "SceneBase.h"
#include "DiffuseShader.hpp"
#include "Pipeline.h"

class NanosuitScene : public SceneBase
{
	using Pipeline = Pipeline<DiffuseShader>;
public:
	Pipeline pipe;

	NanosuitScene(uint32_t w, uint32_t h);

	// Í¨¹ý SceneBase ¼Ì³Ð
	virtual const unsigned char* GetFrameBuffer() const override;
	virtual void Init() override;
	virtual void Draw() override;
};

