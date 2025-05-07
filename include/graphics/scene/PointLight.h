#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "glm/glm.hpp"

#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    struct PointLight
    {
        PointLight() = default;

        PointLight(
            const glm::vec3& color,
            const glm::vec3& worldPos = glm::zero<glm::vec3>(),
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.09f,
            float attenuationQuadratic = 0.032f
        );

        glm::vec3 Color = { 0.f, 0.f, 0.f };
        Transform Transformer;
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.09f;
        float AttenuationQuadratic = 0.032f;
    };
}

#endif