#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class TangentShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec4 normal;
		glm::vec2 texcoord;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			normal += rhs.normal;
			texcoord += rhs.texcoord;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			normal *= v;
			texcoord *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			
			normal = v0.normal * a + v1.normal * b + v2.normal * c;
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			return {
				proj_view * model * glm::vec4(v.position, 1.0f),
				obj_to_world_normal * glm::vec4(v.normal, 0.0f),
				v.texcoord
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
			glm::vec4 color = material->normal->Sample(v.texcoord.x, v.texcoord.y);
			return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

