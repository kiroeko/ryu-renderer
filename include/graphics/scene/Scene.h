#ifndef __SCENE_H__
#define __SCENE_H__

#include "glm/glm.hpp"

namespace RyuRenderer::Graphics::Scene
{
    class Scene
    {
    public:
        static glm::vec3 GetNegativeZAxisDirection();

        static glm::vec3 GetZAxisDirection();

        static glm::vec3 GetNegativeXAxisDirection();

        static glm::vec3 GetXAxisDirection();

        static glm::vec3 GetYAxisDirection();

        static glm::vec3 GetNegativeYAxisDirection();

        static glm::vec3 GetFrontDirection();

        static glm::vec3 GetBackDirection();

        static glm::vec3 GetLeftDirection();

        static glm::vec3 GetRightDirection();

        static glm::vec3 GetUpDirection();

        static glm::vec3 GetDownDirection();
    };
}

#endif