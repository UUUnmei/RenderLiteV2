#pragma once

#include "Vertex.h"
#include "SceneContext.h"
#include "VertexShaderMatHelper.h"
#include "FrameBuffer.h"
#include "PointLight.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
// https://developer.download.nvidia.cn/whitepapers/2008/PCSS_Integration.pdf
// http://www.opengl-tutorial.org/cn/intermediate-tutorials/tutorial-16-shadow-mapping
// https://developer.download.nvidia.cn/shaderlibrary/docs/shadow_PCSS.pdf
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

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			normal = v0.normal * a + v1.normal * b + v2.normal * c;
			texcoord = v0.texcoord * a + v1.texcoord * b + v2.texcoord * c;
			world_pos = v0.world_pos * a + v1.world_pos * b + v2.world_pos * c;
			pos_from_light = v0.pos_from_light * a + v1.pos_from_light * b + v2.pos_from_light * c;
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
				obj_to_world_normal * glm::vec4(v.normal, 0.0f),
				v.texcoord,
				glm::vec3(world_pos),
				light_mvp * glm::vec4(v.position, 1.0f)
			};
		}
	};

	class PixelShader {
		glm::vec2 poissonDisk[27] = {
{-0.08442619, 0.00000001},   // generated with "rand(seed) = 0.5"
{0.09743761, -0.10327788},
{0.01119006, 0.19212449},
{-0.18292630, -0.15349336},
{0.28038719, -0.03277263},
{-0.19327696, 0.25961623},
{-0.06309206, -0.35780966},
{0.33553284, 0.22068308},
{-0.42686638, 0.10116915},
{0.23738207, -0.41115695},
{0.14625250, 0.48852041},
{-0.48643145, -0.24429660},
{0.54315162, -0.19768898},
{-0.24202490, 0.56107038},
{-0.25487268, -0.59086937},
{0.63467598, 0.23100731},
{-0.63164681, 0.31721976},
{0.21160516, -0.70679152},
{0.38415477, 0.66538811},
{-0.77692944, -0.18414302},
{0.69196564, -0.45510325},
{-0.14893284, 0.84458780},
{-0.52948785, -0.71124256},
{0.90926069, 0.10628858},
{-0.72308743, 0.60672635},
{0.05653549, -0.97044605},
{0.68623090, 0.72738355}
		};
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
		float LIGHT_WORLD_SIZE = 1.0;  //size of light mesh
		float LIGHT_FRUSTUM_SIZE = 200.0; // frustum height==width
		float LIGHT_SIZE_UV; // = (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_SIZE);
		float NEAR_PLANE = 1.0f;
		float dnl;
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
			glm::vec3 light_pos = pContext->light->GetPosition();
			glm::vec3 light_intensity = pContext->light->GetIntensity();
			glm::vec3 camera_pos = pContext->camera_pos_cache;
			glm::vec3 light_dir = pContext->light->GetDirection(v.world_pos);
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

			glm::vec3 ambient = ka * glm::vec3(0.5f); //ka * ambient light intensity
			dnl = glm::dot(N, light_dir);
			glm::vec3 diffuse = kd * light_intensity * std::max(0.0f, dnl);

			glm::vec3 half = glm::normalize(view_dir + light_dir);
			glm::vec3 specular = ks * light_intensity * qpow(std::max(0.0f, glm::dot(N, half)), 150);

			//glm::vec3 color = ambient + (diffuse + specular) * visibility;
			glm::vec3 color = (ambient + diffuse + specular) * visibility;
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

		float Bias(float dnl, float invw) {
			float bias = std::clamp(0.008f * tan(acos(dnl)), 0.005f, 0.01f);
			return bias * invw;
		}

		float PenumbraRatio(float zReceiver, float zBlocker) {
			return (zReceiver - zBlocker) / zBlocker;
		}

		float FindBlocker(const glm::vec4& shadowCoord) {
			int cnt = 0;
			float sumZ = 0.0;
			float zReceiver = shadowCoord.z;
			glm::vec2 uv(shadowCoord);
			float searchRadius = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver;
			for (int i = 0; i < SAMPLES; i++) {
				glm::vec2 cur = uv + poissonDisk[i] * searchRadius;
				float Z = LookUpShadowMap(cur);
				if (Z < zReceiver - Bias(dnl, shadowCoord.w)) {
					sumZ += Z;
					cnt++;
				}
			}
			return cnt > 0.0 ? sumZ / cnt : -1.0;
		}

		float LookUpShadowMap(const glm::vec2& shadowCoord) {
			float x = shadowCoord.x * pContext->light->GetShadowMap(0)->GetWidth();
			float y = shadowCoord.y * pContext->light->GetShadowMap(0)->GetHeight();
			glm::vec4 vZ;
			pContext->light->GetShadowMap(0)->read(x, y, vZ);
			float fZ = DecodeFloatFromRGBA(vZ);
			return fZ;
		}

		float PCSS(const glm::vec4& shadowCoord) {
			if (shadowCoord.z > 1.0f) return 1.0f;

	//! MUST initialize pContext->light with a PointLight object
	//or directly use derived class in SceneContext
			auto p = std::dynamic_pointer_cast<PointLight>(pContext->light);
			LIGHT_WORLD_SIZE = p->GetLightWorldSize();
			LIGHT_FRUSTUM_SIZE = p->GetLightFrustumSize();
			NEAR_PLANE = p->GetNearZ();
			LIGHT_SIZE_UV = (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_SIZE);

			//! if sample every time, down the performence a lot
			//poissonSampler(shadowCoord);  
	
			// STEP 1: avgblocker depth
			float zBlocker = FindBlocker(shadowCoord);
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
				if (Z < shadowCoord.z - Bias(dnl, shadowCoord.w))
					cnt += 1.0;
			}
			return 1.0 - cnt / float(SAMPLES);
		}

		float visibility;
		glm::vec4 operator()(const VSOut& v, int modelId, int meshId)
		{

			glm::vec4 shadowCoord = v.pos_from_light / v.pos_from_light.w;
			shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
			shadowCoord.y = -shadowCoord.y * 0.5f + 0.5f;
			shadowCoord.z = shadowCoord.z * 0.5f + 0.5f;
			shadowCoord.w = 1.0f / v.pos_from_light.w;
			visibility = PCSS(shadowCoord);

			glm::vec4 color = BlinnPhong(v, modelId, meshId);

			return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;
};

