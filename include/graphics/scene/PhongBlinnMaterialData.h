#ifndef __PHONGBLINNMATERIALDATA_H__
#define __PHONGBLINNMATERIALDATA_H__

#include <memory>

#include "graphics/Texture2d.h"
#include "graphics/scene/DirectionalLight.h"
#include "graphics/scene/PointLight.h"
#include "graphics/scene/SpotLight.h"

namespace RyuRenderer::Graphics::Scene
{
    struct PhongBlinnMaterialData
    {
        PhongBlinnMaterialData() = default;

        glm::mat4 View = glm::identity<glm::mat4>();
        glm::mat4 Projection = glm::identity<glm::mat4>();
        glm::mat4 Model = glm::identity<glm::mat4>();

        const DirectionalLight* DirectionLight = nullptr;
        const std::vector<PointLight>* PointLights = nullptr;
        const std::vector<SpotLight>* SpotLights = nullptr;

        glm::vec3 Ambient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<const Graphics::Texture2d> Diffuse = nullptr;
        std::shared_ptr<const Graphics::Texture2d> Specular = nullptr;
        float Shininess = 128.f;
        std::shared_ptr<const Graphics::Texture2d> Emission = nullptr;
    };
}

#endif