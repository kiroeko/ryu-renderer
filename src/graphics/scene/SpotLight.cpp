#include "graphics/scene/SpotLight.h"

#include "glm/gtc/matrix_transform.hpp"

namespace RyuRenderer::Graphics::Scene
{
    SpotLight::SpotLight(
        const glm::vec3& color,
        const glm::vec3& worldPos,
        const glm::vec3& direction,
        float innerCutOffDegree,
        float outerCutOffDegree,
        float attenuationConstant,
        float attenuationLinear,
        float attenuationQuadratic
    )
    {
        Color = color;
        Transformer.MoveTo(worldPos);
        Transformer.RotateTo(direction);
        InnerCutOffCos = glm::cos(glm::radians(innerCutOffDegree));
        OuterCutOffCos = glm::cos(glm::radians(outerCutOffDegree));
        AttenuationConstant = attenuationConstant;
        AttenuationLinear = attenuationLinear;
        AttenuationQuadratic = attenuationQuadratic;
    }

    glm::mat4 SpotLight::GetModel() const
    {
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::translate(model, Transformer.GetPosition());
        return model;
    }
}