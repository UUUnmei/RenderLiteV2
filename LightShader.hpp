#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class LightShader
{
public:
	using UseDerivative = std::false_type;
	struct VSOut {
		glm::vec4 proj_pos;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {

		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			return {
				proj_view * model * glm::vec4(v.position, 1.0f),
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;
		glm::vec4 color{1.0f};

		glm::vec4 operator()(const VSOut& v, const VSOut& ddx, const VSOut& ddy, int modelId, int meshId) const
		{
			return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

