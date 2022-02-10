#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class SkyboxShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec3 pos;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			pos += rhs.pos;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			pos *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			pos = v0.pos * a + v1.pos * b + v2.pos * c;
		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			glm::vec4 proj_pos = proj_view * glm::vec4(v.position, 0.0f);
			proj_pos.z = proj_pos.w;
			return {
				proj_pos,
				v.position
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		glm::vec4 operator()(const VSOut& v, int modelId, int meshId) const
		{
			glm::vec4 color = pContext->skybox->Sample(v.pos.x, v.pos.y, v.pos.z);
			return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

