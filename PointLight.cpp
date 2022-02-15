#include "PointLight.h"

PointLight::PointLight(const glm::vec3& pos)
    : mesh(Mesh::GenCube(size))
{
    nearz = 1.0f;
    projection = glm::perspective(fov, aspect, nearz, farz);
    WithPosition(pos);
    intensity = glm::vec3(2500.0f);
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
    view = glm::lookAt(position, /*glm::vec3(0.0f) */ position - glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    vp = projection * view;
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
    vp = projection * view;
    return *this;
}

LightBase& PointLight::WithFarZ(const float z)
{
    farz = z;
    projection = glm::perspective(fov, aspect, nearz, farz);
    vp = projection * view;
    return *this;
}

glm::mat4 PointLight::GetLightMVP(const glm::mat4& model)
{
    return vp * model;
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
