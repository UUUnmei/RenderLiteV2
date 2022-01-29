#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"
#include "FrameBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class PCSS
{
public:

	struct VSOut {
		glm::vec4 proj_pos;
		glm::vec4 normal;
		glm::vec2 texcoord;
		glm::vec3 world_pos;
		glm::vec4 pos_from_light;

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			normal += rhs.normal;
			texcoord += rhs.texcoord;
			world_pos += rhs.world_pos;
			pos_from_light += rhs.pos_from_light;
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
			pos_from_light *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		static VSOut Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			VSOut ret;
			ret.proj_pos = v0.proj_pos * a + v1.proj_pos * b + v2.proj_pos * c;
			ret.normal = v0.normal * a + v1.normal * b + v2.normal * c;
			ret.texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			ret.world_pos = v0.world_pos * a + v1.world_pos * b + v2.world_pos * c;
			ret.pos_from_light = v0.pos_from_light * a + v1.pos_from_light * b + v2.pos_from_light * c;
			return ret;
		}

	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		glm::mat4 light_mvp;

		VSOut operator()(const Vertex& v) const
		{
			glm::vec4 world_pos = model * glm::vec4(v.position, 1.0f);
			return {
				proj_view * world_pos,
				model * glm::vec4(v.normal, 0.0f),
				v.texcoord,
				glm::vec3(world_pos),
				light_mvp * glm::vec4(v.position, 1.0f)
			};
		}
	};

	class PixelShader {
		glm::vec2 poissonDisk[27];
		static constexpr int RINGS = 10;
		static constexpr int SAMPLES = 27;
		static constexpr float PI = 3.1415926;
		float rand(const glm::vec3& co) 
		{ // -1 ~ 1
			return glm::fract(glm::sin(glm::dot(glm::vec2(co) * co.z, glm::vec2(12.9898, 78.233))) * 43758.5453);
		}
		void poissonSampler(const glm::vec4& seed) {
			// calc sampling points with poisson
			float angleStep = PI * 2 * float(RINGS) / float(SAMPLES);
			float angle = rand(seed) * PI * 2;

			float radius = 1.0 / float(SAMPLES);
			float radiusStep = radius;

			for (int i = 0; i < SAMPLES; i++) {
				poissonDisk[i] = glm::vec2(cos(angle), sin(angle)) * pow(radius, 0.75f);
				radius += radiusStep;
				angle += angleStep;
			}
		}
		static constexpr float LIGHT_WORLD_SIZE = 0.4;  //size of light mesh cube
		static constexpr float LIGHT_FRUSTUM_SIZE = 200.0;
		static constexpr float LIGHT_SIZE_UV = (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_SIZE);
		static constexpr float NEAR_PLANE = 1.0f;

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

		glm::vec4 BlinnPhong(const VSOut& v, int modelId, int meshId) {
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

		float DecodeFloatFromRGBA(const glm::vec4& rgba)
		{
			return glm::dot(rgba, glm::vec4(1.0f, 1 / 255.0f, 1 / 65025.0f, 1 / 16581375.0f));
		}

		float PenumbraRatio(float zReceiver, float zBlocker) {
			return (zReceiver - zBlocker) / zBlocker;
		}

		float FindBlocker(const glm::vec2& uv, float zReceiver) {
			int cnt = 0;
			float sumZ = 0.0;
			float searchRadius = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver;
			for (int i = 0; i < SAMPLES; i++) {
				glm::vec2 cur = uv + poissonDisk[i] * searchRadius;
				float Z = LookUpShadowMap(cur);
				if (Z < zReceiver) {
					sumZ += Z;
					cnt ++;
				}
			}
			return cnt > 0.0 ? sumZ / cnt : -1.0;
		}

		float LookUpShadowMap(const glm::vec2& shadowCoord) {
			float x = shadowCoord.x * pContext->GetShadowMapPointer()->GetWidth();
			float y = shadowCoord.y * pContext->GetShadowMapPointer()->GetHeight();
			glm::vec4 vZ;
			pContext->GetShadowMapPointer()->read(x, y, vZ);
			float fZ = DecodeFloatFromRGBA(vZ);
			return fZ;
		}

		float PCSS(const glm::vec4& shadowCoord) {
			poissonSampler(shadowCoord);
			// STEP 1: avgblocker depth
			float zBlocker = FindBlocker(glm::vec2(shadowCoord), shadowCoord.z);
			if (zBlocker < 0.0) return 1.0;
			// STEP 2: penumbra size
			float penumbraRadius = PenumbraRatio(shadowCoord.z, zBlocker)
				* LIGHT_SIZE_UV * NEAR_PLANE / shadowCoord.z;
			// STEP 3: filtering
			// PCF
			float Radius = penumbraRadius;
			float cnt = 0.0;
			for (int i = 0; i < SAMPLES; i++) {
				glm::vec2 cur = poissonDisk[i] * Radius + glm::vec2(shadowCoord);
				float Z = LookUpShadowMap(cur);
				if (Z < shadowCoord.z - 0.01)
					cnt += 1.0;
			}
			return 1.0 - cnt / float(SAMPLES);
		}


		glm::vec4 operator()(const VSOut& v, int modelId, int meshId)
		{
			glm::vec4 color = BlinnPhong(v, modelId, meshId);

			glm::vec4 shadowCoord = v.pos_from_light / v.pos_from_light.w;
			shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
			shadowCoord.y = -shadowCoord.y * 0.5f + 0.5f;
			shadowCoord.z = shadowCoord.z * 0.5f + 0.5f;
			float visibility = PCSS(shadowCoord);

			return visibility * color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;


	void BindLigthMVP(const glm::mat4& mat) {
		vs.light_mvp = mat;
	}
};

