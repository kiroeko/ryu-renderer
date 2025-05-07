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
}