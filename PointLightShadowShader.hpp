#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class PointLightShadowShader
{
public:
	using UseDerivative = std::false_type;
	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec3 world_pos;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			world_pos += rhs.world_pos;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			world_pos *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			world_pos = v0.world_pos * a + v1.world_pos * b + v2.world_pos * c;
		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		glm::mat4 light_mvp;
		
		VSOut operator()(const Vertex& v) const
		{
			return {
				light_mvp * glm::vec4(v.position, 1.0f),
				model * glm::vec4(v.position, 1.0f)
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		// http://www.klayge.org/2013/08/27/%e6%8a%8afloat%e7%bc%96%e7%a0%81%e5%88%b0rgba8/
		glm::vec4 EncodeFloatToRGBA(float v)
		{
			glm::vec4 ret = glm::vec4(1.0f, 255.0f, 65025.0f, 16581375.0f) * v;
			ret = glm::fract(ret);
			glm::vec4 t(ret.y, ret.z, ret.w, ret.w);  // ret.yzww
			ret -= t * glm::vec4(1 / 255.0f, 1 / 255.0f, 1 / 255.0f, 0);
			return ret;
		}

		glm::vec4 operator()(const VSOut& v, const VSOut& ddx, const VSOut& ddy, int modelId, int meshId)
		{	
			float d = glm::length(v.world_pos - pContext->light->GetPosition()) / pContext->light->GetFarZ();
			return EncodeFloatToRGBA(d);
		}

	};

public:
	VertexShader vs;
	PixelShader ps;

};

