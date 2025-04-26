#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

#include "graphics/scene/Scene.h"
#include "graphics/scene/Transform.h"
#include "app/events/WindowEvent.h"
#include "app/events/MouseEvent.h"
#include "app/events/KeyEvent.h"

namespace RyuRenderer::Graphics::Scene
{
    class Camera
    {
    public:
        Camera() = default;
    
        Camera(
            const glm::vec3& position,
            const glm::vec3& frontDir,
            const glm::vec3& upDir,
            float nearPlaneDis = 0.01f,
            float farPlaneDis = 1000000.f,
            float fov = 60.f,
            float aspect = 1.777777f,
            bool isAspectPriority = false,
            bool isVPriority = true
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
    
        Camera(
            const glm::vec3& position,
            const glm::vec3& frontDir,
            const glm::vec3& upDir,
            float nearPlaneDis = 0.01f,
            float farPlaneDis = 1000000.f,
            float vfov = 60.f,
            float hfov = 60.f
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
    
        Camera(
            const glm::vec3& position,
            const glm::vec3& frontDir,
            const glm::vec3& upDir,
            bool orthoMark,
            float nearPlaneDis = 0.01f,
            float farPlaneDis = 1000000.f,
            float orthoW = 10.f,
            float orthoH = 10.f,
            bool isAspectPriority = false,
            bool isOrthoWPriority = true
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
    
        void SetNearPlane(const float& nearPlaneDis)
        {
            nearPlane = nearPlaneDis;
        }
    
        void SetFarPlane(const float& farPlaneDis)
        {
            farPlane = farPlaneDis;
        }
    
        void SetIsPerspective(bool isPerspect)
        {
            isPerspective = isPerspect;
        }
    
        void SetVFov(float vfov)
        {
            if (isAspectRatioPriority && !isVFOVPriority)
                return;
    
            vFOV = vfov;
            MaintenForAspectRatio();
        }
    
        void SetHFov(float hfov)
        {
            if (isAspectRatioPriority && isVFOVPriority)
                return;
    
            hFOV = hfov;
            MaintenForAspectRatio();
        }
    
        void SetOrthoWidth(float orthoW)
        {
            if (isAspectRatioPriority && !isOrthoWidthPriority)
                return;
    
            orthoWidth = orthoW;
            MaintenForAspectRatio();
        }
    
        void SetOrthoHeight(float orthoH)
        {
            if (isAspectRatioPriority && isOrthoWidthPriority)
                return;
    
            orthoHeight = orthoH;
            MaintenForAspectRatio();
        }
    
        void SetIsAspectRatioPriority(bool isAspectPriority)
        {
            isAspectRatioPriority = isAspectPriority;
            MaintenForAspectRatio();
        }
    
        void SetAspectRatio(float aspect)
        {
            if (!isAspectRatioPriority)
                return;
    
            if (aspect >= 0.f)
                aspectRatio = aspect;
            else
                aspectRatio = 0.f;
    
            MaintenForAspectRatio();
        }
    
        void SetIsVFOVPriority(bool isVPriority)
        {
            isVFOVPriority = isVPriority;
            MaintenForAspectRatio();
        }
    
        void SetIsOrthoWidthPriority(bool isOrthoWPriority)
        {
            isOrthoWidthPriority = isOrthoWPriority;
            MaintenForAspectRatio();
        }
    
        glm::mat4 GetView() const
        {
            return glm::lookAt(
                Transformer.GetPosition(),
                Transformer.GetPosition() + Transformer.GetFrontDirection(),
                Transformer.GetUpDirection()
            );
        }
    
        glm::mat4 GetProjection() const
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
    
        float GetNearPlane() const
        {
            return nearPlane;
        }
    
        float GetFarPlane() const
        {
            return farPlane;
        }
    
        bool GetIsPerspective() const
        {
            return isPerspective;
        }
    
        float GetVFov() const
        {
            return vFOV;
        }
    
        float GetHFov() const
        {
            return hFOV;
        }
    
        float GetOrthoWidth() const
        {
            return orthoWidth;
        }
    
        float GetOrthoHeight() const
        {
            return orthoHeight;
        }
    
        bool GetIsAspectRatioPriority()
        {
            return isAspectRatioPriority;
        }
    
        float GetAspectRatio() const
        {
            return aspectRatio;
        }
    
        bool GetIsVFOVPriority()
        {
            return isVFOVPriority;
        }
    
        bool GetIsOrthoWidthPriority()
        {
            return isOrthoWidthPriority;
        }
    
        bool IsAspectVaild()
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
        void OnTick(double deltaTimeInS)
        {
            constexpr float speed = 10.f;
            float distance = deltaTimeInS * speed;
    
            constexpr float epsilon = 1e-6f;
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
    
        void OnMouseMove(const App::Events::MouseEvent& e)
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
    
            constexpr float sensitivityP = 0.02f;
            constexpr float sensitivityY = 0.05f;
            float pitchDegree = -1 * pitchOffset * sensitivityP;

            if (pitchDegree <= -90.f)
                pitchDegree = -89.f;
            else if (pitchDegree >= 90.f)
                pitchDegree = 89.f;

            float yawDegree = yawOffset * sensitivityY;
    
            Transformer.Rotate(Transformer.GetRightDirection(), pitchDegree);
            Transformer.Rotate(Graphics::Scene::Scene::GetDownDirection(), yawDegree);
        }
    
        void OnKeyEvent(const App::Events::KeyEvent& e)
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

        Transform Transformer;
    private:
        static float GetHFOV(float vfovDegree, float aspectRatio)
        {
            float verticalFovRad = glm::radians(vfovDegree);
            float horizontalFovRad = 2.0f * std::atan(std::tan(verticalFovRad / 2.0f) * aspectRatio);
            return glm::degrees(horizontalFovRad);
        }
    
        static float GetVFOV(float hfovDegree, float aspectRatio)
        {
            float horizontalFovRad = glm::radians(hfovDegree);
            float verticalFovRad = 2.0f * std::atan(std::tan(horizontalFovRad / 2.0f) / aspectRatio);
            return glm::degrees(verticalFovRad);
        }
    
        void MaintenForAspectRatio()
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
    
        float nearPlane = 0.f;
        float farPlane = 0.f;
    
        bool isPerspective = true;
    
        float vFOV = 0.f;
        float hFOV = 0.f;
    
        float orthoWidth = 0.f;
        float orthoHeight = 0.f;
    
        float aspectRatio = 0.f;
        // 如果为真，当更改 aspectRatio 时，将自动调整 vFOV / hFOV / orthoWidth / orthoHeight 来让它们符合 aspectRatio 的值；
        //     否则，则自动调整 aspectRatio 让它符合 vFOV / hFOV / orthoWidth / orthoHeight 的值。
        bool isAspectRatioPriority = false;
        bool isVFOVPriority = true;
        bool isOrthoWidthPriority = true;
    
        // Controll
        bool isWKeyHolding = false;
        bool isSKeyHolding = false;
        bool isAKeyHolding = false;
        bool isDKeyHolding = false;
    };
}

#endif