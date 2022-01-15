#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class PhongShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec4 normal;
		glm::vec2 texcoord;
		glm::vec3 world_pos;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			normal += rhs.normal;
			texcoord += rhs.texcoord;
			world_pos += rhs.world_pos;
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
			world_pos *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		static VSOut Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			VSOut ret;
			ret.normal = v0.normal * a + v1.normal * b + v2.normal * c;
			ret.texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			ret.world_pos = v0.world_pos * a + v1.world_pos * b + v2.world_pos * c;
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
				v.texcoord,
				v.position
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		float qpow(float x, int n) const {
			float res = 1;
			while (n) {
				if (n & 1) {
					res *= x;
				}
				x *= x;
				n >>= 1;
			}
			return res;
		}

		glm::vec4 operator()(const VSOut& v, int modelId, int meshId) const
		{
			glm::vec3 light_pos(20.0f);
			glm::vec3 light_intensity(700.0f);
			glm::vec3 camera_pos = pContext->camera_pos_cache;
			glm::vec3 light_dir = glm::normalize(light_pos - v.world_pos);
			float d = glm::length(light_pos - v.world_pos);
			light_intensity /= d * d;
			glm::vec3 N = glm::normalize(glm::vec3(v.normal.x, v.normal.y, v.normal.z));
			glm::vec3 view_dir = glm::normalize(camera_pos - v.world_pos);

			auto material_id = pContext->models[modelId]->meshes[meshId].material_idx;
			auto& material = pContext->models[modelId]->materials[material_id];

			glm::vec3 ka, kd, ks;
			ka = material->Ka;  
			if (material->diffuse != nullptr)
				kd = material->diffuse->Sample(v.texcoord.x, v.texcoord.y);
			else
				kd = material->Kd;
			if (material->specular != nullptr)
				ks = material->specular->Sample(v.texcoord.x, v.texcoord.y);
			else
				ks = material->Ks;

			glm::vec3 ambient = ka * glm::vec3(10.0f); //ka * ambient light intensity
			
			glm::vec3 diffuse = kd * light_intensity * std::max(0.0f, glm::dot(N, light_dir));

			glm::vec3 half = glm::normalize(view_dir + light_dir);
			glm::vec3 specular = ks * light_intensity * qpow(std::max(0.0f, glm::dot(N, half)), 150);

			return glm::vec4(ambient + diffuse + specular, 1.0f);
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

