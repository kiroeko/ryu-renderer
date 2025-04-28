#include "graphics/scene/Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

#include "graphics/scene/Scene.h"

namespace RyuRenderer::Graphics::Scene
{
    Camera::Camera(
        const glm::vec3& position,
        const glm::vec3& frontDir,
        const glm::vec3& upDir,
        float nearPlaneDis,
        float farPlaneDis,
        float fov,
        float aspect,
        bool isAspectPriority,
        bool isVPriority
    )
    {
        Transformer.MoveTo(position);
        Transformer.RotateTo(frontDir, upDir);
        nearPlane = nearPlaneDis;
        farPlane = farPlaneDis;

        isPerspective = true;

        aspectRatio = aspect;

        isAspectRatioPriority = isAspectPriority;
        if (isAspectRatioPriority)
        {
            isVFOVPriority = isVPriority;
            if (isVFOVPriority)
            {
                vFOV = fov;
                hFOV = GetHFOV(vFOV, aspectRatio);
            }
            else
            {
                hFOV = fov;
                vFOV = GetVFOV(hFOV, aspectRatio);
            }
        }
        else
        {
            vFOV = fov;
            hFOV = GetHFOV(vFOV, aspectRatio);
        }
    }

    Camera::Camera(
        const glm::vec3& position,
        const glm::vec3& frontDir,
        const glm::vec3& upDir,
        float nearPlaneDis,
        float farPlaneDis,
        float vfov,
        float hfov
    )
    {
        Transformer.MoveTo(position);
        Transformer.RotateTo(frontDir, upDir);
        nearPlane = nearPlaneDis;
        farPlane = farPlaneDis;

        isPerspective = true;

        vFOV = vfov;
        hFOV = hfov;
        aspectRatio = std::tan(glm::radians(hFOV) / 2.0f) / std::tan(glm::radians(vFOV) / 2.0f);
    }

    Camera::Camera(
        const glm::vec3& position,
        const glm::vec3& frontDir,
        const glm::vec3& upDir,
        bool orthoMark,
        float nearPlaneDis,
        float farPlaneDis,
        float orthoW,
        float orthoH,
        bool isAspectPriority,
        bool isOrthoWPriority
    )
    {
        Transformer.MoveTo(position);
        Transformer.RotateTo(frontDir, upDir);
        nearPlane = nearPlaneDis;
        farPlane = farPlaneDis;

        isPerspective = false;

        orthoWidth = orthoW;
        orthoHeight = orthoH;
        aspectRatio = orthoW / orthoH;

        isAspectRatioPriority = isAspectPriority;
        isOrthoWidthPriority = isOrthoWPriority;
    }

    void Camera::SetNearPlane(const float& nearPlaneDis)
    {
        nearPlane = nearPlaneDis;
    }

    void Camera::SetFarPlane(const float& farPlaneDis)
    {
        farPlane = farPlaneDis;
    }

    void Camera::SetIsPerspective(bool isPerspect)
    {
        isPerspective = isPerspect;
    }

    void Camera::SetVFov(float vfov)
    {
        if (isAspectRatioPriority && !isVFOVPriority)
            return;

        vFOV = vfov;
        MaintenForAspectRatio();
    }

    void Camera::SetHFov(float hfov)
    {
        if (isAspectRatioPriority && isVFOVPriority)
            return;

        hFOV = hfov;
        MaintenForAspectRatio();
    }

    void Camera::SetOrthoWidth(float orthoW)
    {
        if (isAspectRatioPriority && !isOrthoWidthPriority)
            return;

        orthoWidth = orthoW;
        MaintenForAspectRatio();
    }

    void Camera::SetOrthoHeight(float orthoH)
    {
        if (isAspectRatioPriority && isOrthoWidthPriority)
            return;

        orthoHeight = orthoH;
        MaintenForAspectRatio();
    }

    void Camera::SetIsAspectRatioPriority(bool isAspectPriority)
    {
        isAspectRatioPriority = isAspectPriority;
        MaintenForAspectRatio();
    }

    void Camera::SetAspectRatio(float aspect)
    {
        if (!isAspectRatioPriority)
            return;

        if (aspect >= 0.f)
            aspectRatio = aspect;
        else
            aspectRatio = 0.f;

        MaintenForAspectRatio();
    }

    void Camera::SetIsVFOVPriority(bool isVPriority)
    {
        isVFOVPriority = isVPriority;
        MaintenForAspectRatio();
    }

    void Camera::SetIsOrthoWidthPriority(bool isOrthoWPriority)
    {
        isOrthoWidthPriority = isOrthoWPriority;
        MaintenForAspectRatio();
    }

    glm::mat4 Camera::GetView() const
    {
        return glm::lookAt(
            Transformer.GetPosition(),
            Transformer.GetPosition() + Transformer.GetFrontDirection(),
            Transformer.GetUpDirection()
        );
    }

    glm::mat4 Camera::GetProjection() const
    {
        if (isPerspective)
        {
            return glm::perspective(
                glm::radians(vFOV),
                aspectRatio,
                nearPlane,
                farPlane
            );
        }
        else
        {
            return glm::ortho(
                -orthoWidth / 2,
                orthoWidth / 2,
                -orthoHeight / 2,
                orthoHeight / 2,
                nearPlane,
                farPlane);
        }
    }

    float Camera::GetNearPlane() const
    {
        return nearPlane;
    }

    float Camera::GetFarPlane() const
    {
        return farPlane;
    }

    bool Camera::GetIsPerspective() const
    {
        return isPerspective;
    }

    float Camera::GetVFov() const
    {
        return vFOV;
    }

    float Camera::GetHFov() const
    {
        return hFOV;
    }

    float Camera::GetOrthoWidth() const
    {
        return orthoWidth;
    }

    float Camera::GetOrthoHeight() const
    {
        return orthoHeight;
    }

    bool Camera::GetIsAspectRatioPriority()
    {
        return isAspectRatioPriority;
    }

    float Camera::GetAspectRatio() const
    {
        return aspectRatio;
    }

    bool Camera::GetIsVFOVPriority()
    {
        return isVFOVPriority;
    }

    bool Camera::GetIsOrthoWidthPriority()
    {
        return isOrthoWidthPriority;
    }

    bool Camera::IsAspectVaild()
    {
        constexpr float epsilon = 1e-6f;
        if (isPerspective)
        {
            auto a = std::tan(glm::radians(hFOV) / 2.0f) / std::tan(glm::radians(vFOV) / 2.0f);
            return std::abs(a - aspectRatio) < epsilon;
        }
        else
        {
            auto a = orthoWidth / orthoHeight;
            return std::abs(a - aspectRatio) < epsilon;
        }
    }

    // Controll
    void Camera::OnTick(double deltaTimeInS)
    {
        float distance = deltaTimeInS * MoveSpeed;

        constexpr float epsilon = glm::epsilon<float>();
        glm::vec3 moveDir = glm::zero<glm::vec3>();
        if (isWKeyHolding)
        {
            moveDir = Transformer.GetFrontDirection();
        }
        else if (isSKeyHolding)
        {
            moveDir = Transformer.GetBackDirection();
        }
        else if (isAKeyHolding)
        {
            moveDir = Transformer.GetLeftDirection();
        }
        else if (isDKeyHolding)
        {
            moveDir = Transformer.GetRightDirection();
        }
        if (glm::length(moveDir) >= epsilon)
        {
            Transformer.Move(moveDir, distance);
        }
    }

    void Camera::OnMouseMove(const App::Events::MouseEvent& e)
    {
        if (e.Event != App::Events::MouseEvent::EventType::MOUSE_MOVE)
            return;

        static bool isFirstMove = true;
        static float lastMouseX = 0.f;
        static float lastMouseY = 0.f;

        if (isFirstMove)
        {
            lastMouseX = e.MoveXPos;
            lastMouseY = e.MoveYPos;
            isFirstMove = false;
        }

        float pitchOffset = e.MoveYPos - lastMouseY;
        float yawOffset = e.MoveXPos - lastMouseX;
        lastMouseX = e.MoveXPos;
        lastMouseY = e.MoveYPos;

        float pitchDegree = -1 * pitchOffset * YawSensitivity;

        if (pitchDegree <= -90.f)
            pitchDegree = -89.f;
        else if (pitchDegree >= 90.f)
            pitchDegree = 89.f;

        float yawDegree = yawOffset * YawSensitivity;

        Transformer.Rotate(Transformer.GetRightDirection(), pitchDegree);
        Transformer.Rotate(Graphics::Scene::Scene::GetDownDirection(), yawDegree);
    }

    void Camera::OnKeyEvent(const App::Events::KeyEvent& e)
    {
        if (e.Action == App::Events::KeyEvent::ActionType::ACTION_PRESS)
        {
            if (e.Key == App::Events::KeyEvent::KeyType::KEY_W)
            {
                isWKeyHolding = true;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_S)
            {
                isSKeyHolding = true;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_A)
            {
                isAKeyHolding = true;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_D)
            {
                isDKeyHolding = true;
            }
        }
        else if (e.Action == App::Events::KeyEvent::ActionType::ACTION_RELEASE)
        {
            if (e.Key == App::Events::KeyEvent::KeyType::KEY_W)
            {
                isWKeyHolding = false;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_S)
            {
                isSKeyHolding = false;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_A)
            {
                isAKeyHolding = false;
            }
            else if (e.Key == App::Events::KeyEvent::KeyType::KEY_D)
            {
                isDKeyHolding = false;
            }
        }
    }

    float Camera::GetHFOV(float vfovDegree, float aspectRatio)
    {
        float verticalFovRad = glm::radians(vfovDegree);
        float horizontalFovRad = 2.0f * std::atan(std::tan(verticalFovRad / 2.0f) * aspectRatio);
        return glm::degrees(horizontalFovRad);
    }

    float Camera::GetVFOV(float hfovDegree, float aspectRatio)
    {
        float horizontalFovRad = glm::radians(hfovDegree);
        float verticalFovRad = 2.0f * std::atan(std::tan(horizontalFovRad / 2.0f) / aspectRatio);
        return glm::degrees(verticalFovRad);
    }

    void Camera::MaintenForAspectRatio()
    {
        if (isAspectRatioPriority)
        {
            if (aspectRatio <= 0.f)
                return;

            if (isPerspective)
            {
                if (isVFOVPriority)
                    hFOV = GetHFOV(vFOV, aspectRatio);
                else
                    vFOV = GetVFOV(hFOV, aspectRatio);
            }
            else
            {
                if (isOrthoWidthPriority)
                    orthoHeight = orthoWidth / aspectRatio;
                else
                    orthoWidth = orthoHeight * aspectRatio;
            }
        }
        else
        {
            if (isPerspective)
            {
                aspectRatio = std::tan(glm::radians(hFOV) / 2.0f) / std::tan(glm::radians(vFOV) / 2.0f);
            }
            else
            {
                aspectRatio = orthoWidth / orthoHeight;
            }
        }
    }
}