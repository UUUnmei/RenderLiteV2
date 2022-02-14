#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class ShadowShader
{
public:

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
		glm::mat4 light_mvp;

		VSOut operator()(const Vertex& v) const
		{
			return {
				light_mvp * glm::vec4(v.position, 1.0f)
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

		glm::vec4 operator()(const VSOut& v, int modelId, int meshId) 
		{
			//std::cout << v.proj_pos.z << '\n';
			return EncodeFloatToRGBA(v.proj_pos.z);
			//return glm::vec4(glm::vec3(v.proj_pos.z), 1.0f);
		}

	};

public:
	VertexShader vs;
	PixelShader ps;

};

