#ifndef __SPOTLIGHT_H__
#define __SPOTLIGHT_H__

#include "glm/glm.hpp"

#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    struct SpotLight
    {
        SpotLight() = default;

        SpotLight(
            const glm::vec3& color,
            const glm::vec3& worldPos = glm::zero<glm::vec3>(),
            const glm::vec3& direction = glm::vec3(0.f, -1.f, 0.f),
            float innerCutOffDegree = 12.5f,
            float outerCutOffDegree = 17.5f,
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.045f,
            float attenuationQuadratic = 0.0075f
        );

        glm::mat4 GetModel() const;

        glm::vec3 Color = { 0.f, 0.f, 0.f };
        Transform Transformer;
        float InnerCutOffCos = 0.976296f;
        float OuterCutOffCos = 0.953717f;
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.045f;
        float AttenuationQuadratic = 0.0075f;
    };
}

#endif