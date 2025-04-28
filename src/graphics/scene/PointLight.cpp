#include "graphics/scene/PointLight.h"

#include "glm/gtc/matrix_transform.hpp"

namespace RyuRenderer::Graphics::Scene
{
    PointLight::PointLight(
        const glm::vec3& color,
        const glm::vec3& worldPos,
        float attenuationConstant,
        float attenuationLinear,
        float attenuationQuadratic
    )
    {
        Color = color;
        Transformer.MoveTo(worldPos);
        AttenuationConstant = attenuationConstant;
        AttenuationLinear = attenuationLinear;
        AttenuationQuadratic = attenuationQuadratic;
    }

    glm::mat4 PointLight::GetModel() const
    {
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::translate(model, Transformer.GetPosition());
        return model;
    }
}