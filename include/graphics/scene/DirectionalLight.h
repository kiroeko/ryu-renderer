#ifndef __DIRECTIONLIGHT_H__
#define __DIRECTIONLIGHT_H__

#include "glm/glm.hpp"

#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    struct DirectionalLight
    {
        DirectionalLight() = default;

        DirectionalLight(
            const glm::vec3& color,
            const glm::vec3& direction = glm::vec3(0.f, -1.f, 0.f)
        );

        glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
        Transform Transformer;
    };
}

#endif