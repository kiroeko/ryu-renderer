#ifndef __SCENE_H__
#define __SCENE_H__

#include "glm/glm.hpp"

namespace RyuRenderer::Graphics::Scene
{
    class Scene
    {
    public:
        static glm::vec3 GetNegativeZAxisDirection()
        {
            return glm::vec3(0.0f, 0.0f, -1.0f);
        }

        static glm::vec3 GetZAxisDirection()
        {
            return glm::vec3(0.0f, 0.0f, 1.0f);
        }

        static glm::vec3 GetNegativeXAxisDirection()
        {
            return glm::vec3(-1.0f, 0.0f, 0.0f);
        }

        static glm::vec3 GetXAxisDirection()
        {
            return glm::vec3(1.0f, 0.0f, 0.0f);
        }

        static glm::vec3 GetYAxisDirection()
        {
            return glm::vec3(0.0f, 1.0f, 0.0f);
        }

        static glm::vec3 GetNegativeYAxisDirection()
        {
            return glm::vec3(0.0f, -1.0f, 0.0f);
        }

        static glm::vec3 GetFrontDirection()
        {
            return GetNegativeZAxisDirection();
        }

        static glm::vec3 GetBackDirection()
        {
            return GetZAxisDirection();
        }

        static glm::vec3 GetLeftDirection()
        {
            return GetNegativeXAxisDirection();
        }

        static glm::vec3 GetRightDirection()
        {
            return GetXAxisDirection();
        }

        static glm::vec3 GetUpDirection()
        {
            return GetYAxisDirection();
        }

        static glm::vec3 GetDownDirection()
        {
            return GetNegativeYAxisDirection();
        }
    };
}

#endif