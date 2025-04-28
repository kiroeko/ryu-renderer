#include "graphics/scene/Transform.h"

#include "graphics/scene/Scene.h"

namespace RyuRenderer::Graphics::Scene
{
    void Transform::Move(const glm::vec3& dir, float distance)
    {
        glm::vec3 moveDir = glm::normalize(dir);
        position += moveDir * distance;
    }

    void Transform::MoveTo(const glm::vec3& pos)
    {
        position = pos;
    }

    void Transform::Rotate(const glm::vec3& rotateAxis, float degree)
    {
        float angle = glm::radians(degree);
        glm::quat rotation = glm::angleAxis(angle, rotateAxis);
        direction = rotation * direction;
    }

    void Transform::RotateTo(
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

    void Transform::RotateTo(
        float degreeX,
        float degreeY,
        float degreeZ)
    {
        glm::quat rotationX = glm::angleAxis(
            glm::radians(degreeX),
            Scene::GetXAxisDirection()
        );
        glm::quat rotationXUp = glm::angleAxis(
            glm::radians(degreeX + 90),
            Scene::GetXAxisDirection()
        );
        glm::quat rotationY = glm::angleAxis(
            glm::radians(degreeY),
            Scene::GetYAxisDirection()
        );
        glm::quat rotationZ = glm::angleAxis(
            glm::radians(degreeY),
            Scene::GetZAxisDirection()
        );
        direction = rotationX * rotationY * rotationZ;
    }

    void Transform::RotateTo(const glm::vec3& targetDirection)
    {
        glm::vec3 newFront = glm::normalize(targetDirection);

        constexpr float epsilon = glm::epsilon<float>();
        if (glm::length(newFront) < epsilon)
            return;

        if (glm::dot(newFront, GetFrontDirection()) > (1.f - epsilon))
            return;

        glm::vec3 tempUp = Scene::GetYAxisDirection();
        if (glm::abs(glm::dot(newFront, tempUp)) > (1.f - epsilon))
        {
            tempUp = Scene::GetZAxisDirection();
        }

        RotateTo(targetDirection, tempUp);
    }

    void Transform::LookAt(
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

    glm::vec3 Transform::GetPosition() const
    {
        return position;
    }

    glm::quat Transform::GetDirection() const
    {
        return direction;
    }

    glm::vec3 Transform::GetFrontDirection() const
    {
        return direction * Scene::GetFrontDirection();
    }

    glm::vec3 Transform::GetBackDirection() const
    {
        return direction * Scene::GetBackDirection();
    }

    glm::vec3 Transform::GetLeftDirection() const
    {
        return direction * Scene::GetLeftDirection();
    }

    glm::vec3 Transform::GetRightDirection() const
    {
        return direction * Scene::GetRightDirection();
    }

    glm::vec3 Transform::GetUpDirection() const
    {
        return direction * Scene::GetUpDirection();
    }

    glm::vec3 Transform::GetDownDirection() const
    {
        return direction * Scene::GetDownDirection();
    }
}