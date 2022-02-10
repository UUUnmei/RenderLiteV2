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

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			normal = v0.normal * a + v1.normal * b + v2.normal * c;
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			world_pos = v0.world_pos * a + v1.world_pos * b + v2.world_pos * c;
		}

		void LerpWithDerivatives(const VSOut& v0, const VSOut& v1, const VSOut& v2, const glm::vec3& ddx, const glm::vec3& ddy) noexcept {

		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			glm::vec4 world_pos = model * glm::vec4(v.position, 1.0f);
			return {
				proj_view * world_pos,
				obj_to_world_normal * glm::vec4(v.normal, 0.0f),
				v.texcoord,
				glm::vec3(world_pos)
			};
		}
	};

	class PixelShader {
	public:
		std::shared_ptr<SceneContext> pContext;

		float qpow(float x, int n) {
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


		glm::vec4 operator()(const VSOut& v, int modelId, int meshId)
		{
			glm::vec3 light_pos = pContext->light->position;
			glm::vec3 light_intensity = pContext->light->intensity;
			glm::vec3 camera_pos = pContext->camera_pos_cache;
			glm::vec3 light_dir = glm::normalize(light_pos /*- glm::vec3(0.0f)*/); // directional light
			//glm::vec3 light_dir = glm::normalize(light_pos - v.world_pos);
			float d = glm::length(light_pos - v.world_pos);
			light_intensity /= d * d;
			glm::vec3 N = glm::normalize(glm::vec3(v.normal));
			glm::vec3 view_dir = glm::normalize(camera_pos - v.world_pos);

			auto material_id = pContext->models[modelId]->meshes[meshId].material_idx;
			auto& material = pContext->models[modelId]->materials[material_id];

			glm::vec3 ka, kd, ks;
			ka = material->Ka;
			if (material->diffuse != nullptr)
				kd = material->diffuse->Sample(v.texcoord.x, v.texcoord.y);
			else
				kd = material->Kd;
			kd = glm::pow(kd, glm::vec3(2.2f));
			if (material->specular != nullptr)
				ks = material->specular->Sample(v.texcoord.x, v.texcoord.y);
			else
				ks = material->Ks;

			glm::vec3 ambient = ka * glm::vec3(1.0f); //ka * ambient light intensity

			glm::vec3 diffuse = kd * light_intensity * std::max(0.0f, glm::dot(N, light_dir));

			glm::vec3 half = glm::normalize(view_dir + light_dir);
			glm::vec3 specular = ks * light_intensity * qpow(std::max(0.0f, glm::dot(N, half)), 150);

			glm::vec3 color = ambient + diffuse + specular;
			color = glm::pow(color, glm::vec3(1.0f / 2.2f));
			color.r = std::max(0.0f, std::min(1.0f, color.r)); // Saturate
			color.g = std::max(0.0f, std::min(1.0f, color.g));
			color.b = std::max(0.0f, std::min(1.0f, color.b));

			return glm::vec4(color, 1.0f);
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

