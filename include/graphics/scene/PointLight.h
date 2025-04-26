#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

#include "graphics/scene/Transform.h"

namespace RyuRenderer::Graphics::Scene
{
    struct PointLight
    {
        PointLight() = default;

        PointLight(
            const glm::vec3& color,
            const glm::vec3& worldPos = glm::zero<glm::vec3>(),
            float mScale = 0.2f,
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.09f,
            float attenuationQuadratic = 0.032f
        )
        {
            Color = color;
            Transformer.MoveTo(worldPos);
            modelScale = mScale;
            AttenuationConstant = attenuationConstant;
            AttenuationLinear = attenuationLinear;
            AttenuationQuadratic = attenuationQuadratic;
        }

        glm::mat4 GetModel() const
        {
            glm::mat4 model = glm::identity<glm::mat4>();
            model = glm::translate(model, Transformer.GetPosition());
            model = glm::scale(model, glm::vec3(modelScale));
            return model;
        }

        glm::vec3 Color = { 0.f, 0.f, 0.f };
        Transform Transformer;
        float modelScale = 1.0f;
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.09f;
        float AttenuationQuadratic = 0.032f;
    };
}

#endif