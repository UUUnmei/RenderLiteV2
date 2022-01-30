#pragma once

#include "SceneBase.h"
#include "OrbitCamera.h"
#include "NormalShader.hpp"
#include "Pipeline.h"

class CubeScene final: public SceneBase
{
	using NormalRenderer = Pipeline<NormalShader>;
public:
	NormalRenderer nrender;
	std::shared_ptr<SceneContext> context;
	
	CubeScene(uint32_t w, uint32_t h);

	

	// Í¨¹ý SceneBase ¼Ì³Ð
	const unsigned char* GetFrameBuffer() const override;
	OrbitCamera& GetCamera() override;
	void Init() override;
	void Draw() override;

	void OnKeyChanged(int key, int scanCode, int action, int mode) override;
	void OnMousePositionChanged(double xpos, double ypos) override;
	void OnMouseButtonChanged(int button, int action, int mode) override;
	void OnScrollChanged(double x, double y) override;

};

