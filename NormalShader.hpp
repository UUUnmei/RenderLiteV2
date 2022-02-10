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
		glm::vec3 pos;  //world position

		VSOut& operator+=(const VSOut& rhs)
		{
			proj_pos += rhs.proj_pos;
			normal += rhs.normal;
			pos += rhs.pos;
			return *this;
		}
		VSOut operator+(const VSOut& rhs) const
		{
			return VSOut(*this) += rhs;
		}

		VSOut& operator*=(float v) {
			proj_pos *= v;
			normal *= v;
			pos *= v;
			return *this;
		}
		VSOut operator*(float rhs) const
		{
			return VSOut(*this) *= rhs;
		}

		void Lerp(const VSOut& v0, const VSOut& v1, const VSOut& v2, float a, float b, float c) noexcept {
			normal = v0.normal * a + v1.normal * b + v2.normal * c;
			pos = v0.pos * a + v1.pos * b + v2.pos * c;
		}
	};


	class VertexShader : public VertexShaderMatHelper {
	public:
		VSOut operator()(const Vertex& v) const
		{
			
			glm::vec4 world = model * glm::vec4(v.position, 1.0f);
			return {
				proj_view * world,
				obj_to_world_normal * glm::vec4(v.normal, 0.0f),
				glm::vec3( world )
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

			// skybox reflect in world space
			//glm::vec3 N = glm::normalize(glm::vec3(v.normal));
			//glm::vec3 I = glm::normalize(v.pos - pContext->camera_pos_cache);
			//glm::vec3 R = glm::reflect(I, N);
			//glm::vec4 color = pContext->skybox->Sample(R.x, R.y, R.z);
			//return color;

			// skybox refract (only once) in world space 
			//material	 ¦Ç
			//air	   1.00
			//water    1.33
			//ice      1.309
			//glass	   1.52
			//diamond  2.42
			//float eta = 1.00 / 1.52;
			//glm::vec3 N = glm::normalize(glm::vec3(v.normal));
			//glm::vec3 I = glm::normalize(v.pos - pContext->camera_pos_cache);
			//glm::vec3 R = glm::refract(I, N, eta);
			//glm::vec4 color = pContext->skybox->Sample(R.x, R.y, R.z);
			//return color;
		}
	};

public:
	VertexShader vs;
	PixelShader ps;

};

