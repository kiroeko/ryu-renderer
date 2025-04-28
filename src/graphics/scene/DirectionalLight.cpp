#include "graphics/scene/DirectionalLight.h"

namespace RyuRenderer::Graphics::Scene
{
    DirectionalLight::DirectionalLight(
        const glm::vec3& color,
        const glm::vec3& direction
    )
    {
        Color = color;
        Transformer.RotateTo(direction);
    }
}