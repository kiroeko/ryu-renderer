#ifndef __SPOTLIGHT_H__
#define __SPOTLIGHT_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

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
            float mScale = 0.2f,
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.045f,
            float attenuationQuadratic = 0.0075f
        )
        {
            Color = color;
            Transformer.MoveTo(worldPos);
            Transformer.RotateTo(direction);
            modelScale = mScale;
            InnerCutOffCos = glm::cos(glm::radians(innerCutOffDegree));
            OuterCutOffCos = glm::cos(glm::radians(outerCutOffDegree));
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
        float InnerCutOffCos = 0.976296f;
        float OuterCutOffCos = 0.953717f;
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.045f;
        float AttenuationQuadratic = 0.0075f;
    };
}

#endif