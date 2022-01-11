#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class NormalShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec4 normal;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			normal += rhs.normal;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			normal *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		static VSOut Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			VSOut ret;
			ret.normal = v0.normal * a + v1.normal * b + v2.normal * c;
			return ret;
		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			return {
				proj_view * model * glm::vec4(v.position, 1.0f),
				it_view_model * glm::vec4(v.normal, 0.0f),
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		glm::vec4 operator()(const VSOut& v, int modelId, int meshId) const
		{
			glm::vec3 color(v.normal.x, v.normal.y, v.normal.z);
			color = glm::normalize(color) * 0.5f + glm::vec3(0.5f);  //·¨Ïß-1~1 -> 0~1 
			return glm::vec4(color, 1.0f);
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

