#pragma once

namespace ConfigParams {
	enum class DrawMode {
		TriangleMesh,
		WireFrame   // NO FACECULL
	};
}

struct PipelineConfig {
	ConfigParams::DrawMode draw_mode = ConfigParams::DrawMode::TriangleMesh;


};