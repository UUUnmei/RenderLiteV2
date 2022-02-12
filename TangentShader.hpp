#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

//#define TANGENT_SPACE
#ifdef TANGENT_SPACE

class TangentShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec2 texcoord;
		glm::vec3 pos_tangent;
		glm::vec3 light_pos_tangent;
		glm::vec3 cam_pos_tangent;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			texcoord += rhs.texcoord;
			pos_tangent += rhs.pos_tangent;
			light_pos_tangent += rhs.light_pos_tangent;
			cam_pos_tangent += rhs.cam_pos_tangent;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			texcoord *= v;
			pos_tangent *= v;
			light_pos_tangent *= v;
			cam_pos_tangent *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			pos_tangent = v0.pos_tangent * a + v1.pos_tangent * b + v2.pos_tangent * c;
			light_pos_tangent = v0.light_pos_tangent * a + v1.light_pos_tangent * b + v2.light_pos_tangent * c;
			cam_pos_tangent = v0.cam_pos_tangent * a + v1.cam_pos_tangent * b + v2.cam_pos_tangent * c;
		}

	};

	// make all attr to tangent space
	class VertexShader : public VertexShaderMatHelper {
	public:
		std::shared_ptr<SceneContext> pContext;
		VSOut operator()(const Vertex& v) const
		{
			glm::vec4 world_pos = model * glm::vec4(v.position, 1.0f);
			glm::vec3 T = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.tangent, 0.0)));
			glm::vec3 B = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.bitangent, 0.0)));
			glm::vec3 N = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.normal, 0.0)));
			glm::mat3 TBN = glm::transpose(glm::mat3(T, B, N));
			return {
				proj_view * world_pos,
				v.texcoord,
				TBN * glm::vec3(world_pos),
				TBN * pContext->light->position,
				TBN * pContext->camera_pos_cache
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
			glm::vec3 light_pos = v.light_pos_tangent;
			glm::vec3 light_intensity = pContext->light->intensity;
			glm::vec3 camera_pos = v.cam_pos_tangent;
			glm::vec3 light_dir = glm::normalize(light_pos - v.pos_tangent); // suppose point light
			float d = glm::length(light_pos - v.pos_tangent);
			light_intensity /= d * d;
			glm::vec3 view_dir = glm::normalize(camera_pos - v.pos_tangent);

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

			assert(material->normal);
			glm::vec3 N = glm::vec3(material->normal->Sample(v.texcoord.x, v.texcoord.y)) * 2.0f - 1.0f;
			N = glm::normalize(N);

			glm::vec3 ambient = ka * glm::vec3(1.0f); //ka * ambient light intensity

			glm::vec3 diffuse = kd * light_intensity * std::max(0.0f, glm::dot(N, light_dir));

			glm::vec3 half = glm::normalize(view_dir + light_dir);
			glm::vec3 specular = ks * light_intensity * qpow(std::max(0.0f, glm::dot(N, half)), 150);

			glm::vec3 color = ambient + diffuse + specular;
			color = glm::pow(color, glm::vec3(1.0f / 2.2f));
			color.r = std::max(0.0f, std::min(1.0f, color.r)); // Saturate
			color.g = std::max(0.0f, std::min(1.0f, color.g));
			color.b = std::max(0.0f, std::min(1.0f, color.b));

			//return glm::vec4(N * 0.5f + 0.5f, 1.0f);
			return glm::vec4(color, 1.0f);
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};
 
#else 


 
class TangentShader
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec3 wrd_pos;
		glm::vec2 texcoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec3 normal;
		

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			wrd_pos += rhs.wrd_pos;
			texcoord += rhs.texcoord;
			tangent += rhs.tangent;
			bitangent += rhs.bitangent;
			normal += rhs.normal;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			wrd_pos *= v;
			texcoord *= v;
			tangent *= v;
			bitangent *= v;
			normal *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			wrd_pos = v0.wrd_pos * a + v1.wrd_pos * b + v2.wrd_pos * c;
			tangent = v0.tangent * a + v1.tangent * b + v2.tangent * c;
			bitangent = v0.bitangent * a + v1.bitangent * b + v2.bitangent * c;
			normal = v0.normal * a + v1.normal * b + v2.normal * c;
		}

	};

   // only make tangent space NORMAL to world space
	class VertexShader : public VertexShaderMatHelper {
	public:
		std::shared_ptr<SceneContext> pContext;
		VSOut operator()(const Vertex& v) const
		{
			glm::vec4 world_pos = model * glm::vec4(v.position, 1.0f);
			glm::vec3 T = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.tangent, 0.0)));
			glm::vec3 N = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.normal, 0.0)));
			glm::vec3 B = glm::normalize(glm::vec3(obj_to_world_normal * glm::vec4(v.bitangent, 0.0)));
			return {
				proj_view * world_pos,
				world_pos,
				v.texcoord,
				T,
				B,
				N
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
			glm::vec3 light_dir = glm::normalize(light_pos - v.wrd_pos); // suppose point light
			float d = glm::length(light_pos - v.wrd_pos);
			light_intensity /= d * d;
			glm::vec3 view_dir = glm::normalize(camera_pos - v.wrd_pos);

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

			glm::vec3 N;
			if (material->normal != nullptr) {
				N = glm::vec3(material->normal->Sample(v.texcoord.x, v.texcoord.y)) * 2.0f - 1.0f;
				glm::vec3 n = glm::normalize(v.normal);
				glm::vec3 t = glm::normalize(v.tangent - glm::dot(n, v.tangent) * n);
				glm::vec3 b = glm::normalize(v.bitangent - glm::dot(n, v.bitangent) * n - glm::dot(t, v.bitangent) * t);
				N = glm::normalize(glm::mat3(t, b, n) * N);
			}
			else {
				N = glm::normalize(v.normal);
			}


			glm::vec3 ambient = ka * glm::vec3(1.0f); //ka * ambient light intensity

			glm::vec3 diffuse = kd * light_intensity * std::max(0.0f, glm::dot(N, light_dir));

			glm::vec3 half = glm::normalize(view_dir + light_dir);
			glm::vec3 specular = ks * light_intensity * qpow(std::max(0.0f, glm::dot(N, half)), 150);

			glm::vec3 color = ambient + diffuse + specular;
			color = glm::pow(color, glm::vec3(1.0f / 2.2f));
			color.r = std::max(0.0f, std::min(1.0f, color.r)); // Saturate
			color.g = std::max(0.0f, std::min(1.0f, color.g));
			color.b = std::max(0.0f, std::min(1.0f, color.b));

			//return glm::vec4(N * 0.5f + 0.5f, 1.0f);
			return glm::vec4(color, 1.0f);
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};


#endif // TANGENT_SPACE
