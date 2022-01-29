#pragma once

namespace ConfigParams {
	enum class DrawMode {
		TriangleMesh,  // DEFAULT
		WireFrame   // NO FACECULL; WITHOUT Z-buffer, ORDER MATTER
	};

	enum class FaceCullOrder {
		CCW,   // DEFAULT, treat CCW as FRONT 
		CW,		//  treat CW as FRONT
		NONE
	};
}

struct PipelineConfig {
	ConfigParams::DrawMode draw_mode = ConfigParams::DrawMode::TriangleMesh;
	ConfigParams::FaceCullOrder fc_order = ConfigParams::FaceCullOrder::CCW;

};