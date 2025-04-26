#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

#include "graphics/scene/Scene.h"

namespace RyuRenderer::Graphics::Scene
{
    class Transform
    {
    public:
        Transform() = default;

        void Move(const glm::vec3& dir, float distance)
        {
            glm::vec3 moveDir = glm::normalize(dir);
            position += moveDir * distance;
        }

        void MoveTo(const glm::vec3& poa)
        {
            position = poa;
        }

        void Rotate(const glm::vec3& rotateAxis, float degree)
        {
            float angle = glm::radians(degree);
            glm::quat rotation = glm::angleAxis(angle, rotateAxis);
            direction = rotation * direction;
        }

        void RotateTo(
            const glm::vec3& frontDir,
            const glm::vec3& upDir)
        {
            glm::vec3 front = glm::normalize(frontDir);
            glm::vec3 up = glm::normalize(upDir);
            glm::vec3 right = glm::normalize(glm::cross(front, up));
            up = glm::normalize(glm::cross(right, front));

            glm::mat3 rotationMatrix = glm::mat3(right, up, -front);
            direction = glm::quat(rotationMatrix);
        }

        void RotateTo(
            float degreeX,
            float degreeY,
            float degreeZ)
        {
            glm::quat rotationX = glm::angleAxis(
                glm::radians(degreeX),
                glm::vec3(1.0f, 0.0f, 0.0f)
            );
            glm::quat rotationXUp = glm::angleAxis(
                glm::radians(degreeX + 90),
                glm::vec3(1.0f, 0.0f, 0.0f)
            );
            glm::quat rotationY = glm::angleAxis(
                glm::radians(degreeY),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            glm::quat rotationZ = glm::angleAxis(
                glm::radians(degreeY),
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            direction = rotationX * rotationY * rotationZ;
        }

        void RotateTo(const glm::vec3& targetDirection)
        {
            glm::vec3 newFront = glm::normalize(targetDirection);

            constexpr float epsilon = glm::epsilon<float>();
            if (glm::length(newFront) < epsilon)
                return;

            if (glm::dot(newFront, GetFrontDirection()) > (1.f - epsilon))
                return;

            glm::vec3 tempUp = glm::vec3(0.0f, 1.0f, 0.0f);
            if (glm::abs(glm::dot(newFront, tempUp)) > (1.f - epsilon))
            {
                tempUp = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            RotateTo(targetDirection, tempUp);
        }

        void LookAt(
            const glm::vec3& targetPos,
            const glm::vec3& upDir)
        {
            glm::vec3 front = glm::normalize(position - targetPos);
            glm::vec3 up = glm::normalize(upDir);
            glm::vec3 right = glm::normalize(glm::cross(front, up));
            up = glm::normalize(glm::cross(right, front));

            glm::mat3 rotationMatrix = glm::mat3(right, up, -front);
            direction = glm::quat(rotationMatrix);
        }

        glm::vec3 GetPosition() const
        {
            return position;
        }

        glm::quat GetDirection() const
        {
            return direction;
        }

        glm::vec3 GetFrontDirection() const
        {
            return direction * glm::vec3(0.f, 0.f, -1.f);
        }

        glm::vec3 GetBackDirection() const
        {
            return direction * glm::vec3(0.f, 0.f, 1.f);
        }

        glm::vec3 GetLeftDirection() const
        {
            return direction * glm::vec3(-1.f, 0.f, 0.f);
        }

        glm::vec3 GetRightDirection() const
        {
            return direction * glm::vec3(1.f, 0.f, 0.f);
        }

        glm::vec3 GetUpDirection() const
        {
            return direction * glm::vec3(0.f, 1.f, 0.f);
        }

        glm::vec3 GetDownDirection() const
        {
            return direction * glm::vec3(0.f, -1.f, 0.f);
        }
    private:
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat direction = glm::identity<glm::quat>();
    };
}

#endif