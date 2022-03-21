#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class DiffuseShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec2 texcoord;

		glm::vec2 ddx_texcoord;
		glm::vec2 ddy_texcoord;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			texcoord += rhs.texcoord;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			texcoord *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
		}

		void LerpWithDerivatives(const VSOut& v0, const VSOut& v1, const VSOut& v2, const glm::vec3& ddx, const glm::vec3& ddy) noexcept {
			ddx_texcoord = v0.texcoord * ddx.x + v1.texcoord * ddx.y + v2.texcoord * ddx.z;
			ddy_texcoord = v0.texcoord * ddy.x + v1.texcoord * ddy.y + v2.texcoord * ddy.z;
		}
	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			return {
				proj_view * model * glm::vec4(v.position, 1.0f),
				v.texcoord,
				glm::vec2{},
				glm::vec2{}
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		glm::vec4 operator()(const VSOut& v, int modelId, int meshId) const
		{
			auto material_id = pContext->models[modelId]->meshes[meshId].material_idx;
			auto& material = pContext->models[modelId]->materials[material_id];
			glm::vec4 color;


			if (material->diffuse != nullptr) {
// use mipmap
// reference
// https://gamedev.stackexchange.com/questions/153530/hlsl-mipmap-sampling-in-pixel-shader
// https://community.khronos.org/t/mipmap-level-calculation-using-dfdx-dfdy/67480/2
				float w = material->diffuse->GetWidth();
				float h = material->diffuse->GetHeight();
				float mx2 = std::max(
					glm::dot(v.ddx_texcoord, v.ddx_texcoord) * w * w,
					glm::dot(v.ddy_texcoord, v.ddy_texcoord) * h * h
				);
				float level = 0.5f * glm::log2(mx2);
				color = material->diffuse->Sample(v.texcoord.x, v.texcoord.y, level);
			}
			else
				color = glm::vec4(material->Kd, 1.0f);
			return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

