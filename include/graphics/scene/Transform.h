#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace RyuRenderer::Graphics::Scene
{
    class Transform
    {
    public:
        Transform() = default;

        bool operator==(const Transform& other) const
        {
            return position == other.position &&
                   rotation == other.rotation &&
                   scale == other.scale;
        }

        void Move(const glm::vec3& dir, float distance);

        void MoveTo(const glm::vec3& pos);

        void Rotate(const glm::vec3& rotateAxis, float degree);

        void RotateTo(
            const glm::vec3& frontDir,
            const glm::vec3& upDir);

        void RotateTo(
            float degreeX,
            float degreeY,
            float degreeZ);

        void RotateTo(const glm::vec3& targetDirection);

        void LookAt(
            const glm::vec3& targetPos,
            const glm::vec3& upDir);

        void Scale(const glm::vec3& s);

        void ScaleTo(const glm::vec3& scaleTarget);

        glm::mat4 GetMatrix() const;

        glm::vec3 GetPosition() const;

        glm::quat GetRotation() const;

        glm::vec3 GetScale() const;

        glm::vec3 GetFrontDirection() const;

        glm::vec3 GetBackDirection() const;

        glm::vec3 GetLeftDirection() const;

        glm::vec3 GetRightDirection() const;

        glm::vec3 GetUpDirection() const;

        glm::vec3 GetDownDirection() const;
    private:
        glm::vec3 position = glm::zero<glm::vec3>();
        glm::quat rotation = glm::identity<glm::quat>();
        glm::vec3 scale = glm::vec3(1.f);
    };
}

#endif