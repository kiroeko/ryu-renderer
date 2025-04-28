#include "graphics/scene/Scene.h"

namespace RyuRenderer::Graphics::Scene
{
    glm::vec3 Scene::GetNegativeZAxisDirection()
    {
        return glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 Scene::GetZAxisDirection()
    {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::vec3 Scene::GetNegativeXAxisDirection()
    {
        return glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Scene::GetXAxisDirection()
    {
        return glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Scene::GetYAxisDirection()
    {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 Scene::GetNegativeYAxisDirection()
    {
        return glm::vec3(0.0f, -1.0f, 0.0f);
    }

    glm::vec3 Scene::GetFrontDirection()
    {
        return GetNegativeZAxisDirection();
    }

    glm::vec3 Scene::GetBackDirection()
    {
        return GetZAxisDirection();
    }

    glm::vec3 Scene::GetLeftDirection()
    {
        return GetNegativeXAxisDirection();
    }

    glm::vec3 Scene::GetRightDirection()
    {
        return GetXAxisDirection();
    }

    glm::vec3 Scene::GetUpDirection()
    {
        return GetYAxisDirection();
    }

    glm::vec3 Scene::GetDownDirection()
    {
        return GetNegativeYAxisDirection();
    }
}