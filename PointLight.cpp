#include "PointLight.h"
#include "SceneContext.h"
#include "ShadowShader.hpp"
#include "Pipeline.h"

void PointLight::UpdateVPs(void)
{
    for (int i = 0; i < 6; ++i)
        vps[i] = projection * glm::lookAt(position, position - directions[i], ups[i]);
}

PointLight::PointLight(const glm::vec3& pos)
    : mesh(Mesh::GenCube(size))
{
    nearz = 0.5f;
    projection = glm::perspective(fov, aspect, nearz, farz);
    WithPosition(pos);
    intensity = glm::vec3(2500.0f);
    for (int i = 0; i < 6; ++i) shadow_map[i] = nullptr;
}

PointLight& PointLight::WithSize(const float r)
{
    assert(r > 0);
    size = r;
    mesh = Mesh::GenCube(std::min(10.0f, size));
    return *this;
}

LightBase& PointLight::WithPosition(const glm::vec3& pos)
{
    position = pos;
    UpdateVPs();
    return *this;
}

LightBase& PointLight::WithIntensity(const glm::vec3& i)
{
    intensity = i;
    return *this;
}

LightBase& PointLight::WithNearZ(const float z)
{
    nearz = z;
    projection = glm::perspective(fov, aspect, nearz, farz);
    UpdateVPs();
    return *this;
}

LightBase& PointLight::WithFarZ(const float z)
{
    farz = z;
    projection = glm::perspective(fov, aspect, nearz, farz);
    UpdateVPs();
    return *this;
}

glm::mat4 PointLight::GetLightMVP(const glm::mat4& model, int slot)
{
    assert(slot >= 0 && slot <= 5);
    return vps[slot] * model;
}

glm::vec3 PointLight::GetDirection(const glm::vec3& pos)
{
    return glm::normalize(position - pos);
}

glm::mat4 PointLight::GetLightModelMatrix(void)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    return model;
}

Mesh& PointLight::GetLightMesh(void)
{
    return mesh;
}

const float PointLight::GetLightWorldSize(void) const
{
    return size;
}

const float PointLight::GetLightFrustumSize(void) const
{
    return 2 * nearz * tanf(fov * 0.5f); //height
}

void PointLight::EnableShadowMap(int scale)
{
    assert(scale == 1 || scale == 2);
    for(int i = 0; i < 6; ++i)
        shadow_map[i] = std::make_unique<FrameBuffer>(1024 * scale, 1024 * scale);
	invH = invW = 1.0f / (1024 * scale);
}

// here shadowCoord is in world space
// because we store distance between vertex and light in world space as depth
float PointLight::LookUpShadowMap(const glm::vec4& shadowCoord)
{
	// similar with skybox
	float x = shadowCoord.x;
	float y = shadowCoord.y;
	float z = shadowCoord.z;
	float absX = fabs(x);
	float absY = fabs(y);
	float absZ = fabs(z);
	float ma, sc, tc;
	int faceIndex;
	ma = std::max(std::max(absX, absY), absZ);
	if (ma == absX) {
		if (x > 0) {
			faceIndex = 5;   // right
			sc = z;
			tc = y;
		}
		else {
			faceIndex = 4;   // left
			sc = -z;
			tc = y;
		}
	}
	else if (ma == absY) {
		if (y > 0) {
			faceIndex = 3;   // top
			sc = x;
			tc = z;
		}
		else {
			faceIndex = 2;   // bottom
			sc = x;
			tc = -z;
		}
	}
	else {
		if (z > 0) {
			faceIndex = 1;  // back
			sc = -x;
			tc = y;
		}
		else {
			faceIndex = 0;    // front
			sc = x;
			tc = y;
		}
	}
	glm::vec2 uv((sc / ma + 1) / 2, (tc / ma + 1) / 2);
	// sampling manually, see texture2d
	// clamp to edge
	x = std::min(1.0f - invW, std::max(invW, x));
	y = std::min(1.0f - invH, std::max(invH, y));
	
	uv.x = uv.x * shadow_map[faceIndex]->GetWidth() + 0.5f;
	uv.y = (1 - uv.y) * shadow_map[faceIndex]->GetHeight() + 0.5f;
	glm::vec4 ret;
	shadow_map[faceIndex]->read(uv.x, uv.y, ret);
	float d = DecodeFloatFromRGBA(ret);
	return d;
}

FrameBuffer* PointLight::GetShadowMap(int slot)
{
    assert(slot >= 0 && slot <= 5); 
    assert(shadow_map[slot]);
    return shadow_map[slot].get();
}
