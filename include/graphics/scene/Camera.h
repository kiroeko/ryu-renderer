#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>

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
            MoveTo(position);
            RotateTo(frontDir, upDir);
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
            MoveTo(position);
            RotateTo(frontDir, upDir);
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
            MoveTo(position);
            RotateTo(frontDir, upDir);
            nearPlane = nearPlaneDis;
            farPlane = farPlaneDis;
    
            isPerspective = false;
    
            orthoWidth = orthoW;
            orthoHeight = orthoH;
            aspectRatio = orthoW / orthoH;
    
            isAspectRatioPriority = isAspectPriority;
            isOrthoWidthPriority = isOrthoWPriority;
        }
    
        void Move(const glm::vec3& direction, float distance)
        {
            glm::vec3 dir = glm::normalize(direction);
            pos += dir * distance;
        }
    
        void MoveTo(const glm::vec3& position)
        {
            pos = position;
        }
    
        void Rotate(const glm::vec3& rotateAxis, float degree)
        {
            float angle = glm::radians(degree);
            glm::quat rotation = glm::angleAxis(angle, rotateAxis);
            front = rotation * front;
        }
    
        void RotateTo(
            const glm::vec3& frontDir,
            const glm::vec3& upDir)
        {
            front = glm::normalize(frontDir);
            up = glm::normalize(upDir);
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
            front = rotationX * rotationY * rotationZ * glm::vec3(0.0f, 0.0f, -1.0f);
            up = rotationXUp * rotationY * rotationZ * glm::vec3(0.0f, 0.0f, -1.0f);
        }
    
        void LookAt(
            const glm::vec3& targetPos,
            const glm::vec3& upDir)
        {
            front = targetPos - pos;
            up = glm::normalize(upDir);
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
                pos,
                pos + front,
                up
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
    
        glm::vec3 GetPos() const
        {
            return pos;
        }
    
        glm::vec3 GetFrontDirection() const
        {
            return front;
        }
    
        glm::vec3 GetBackDirection() const
        {
            return front * -1.f;
        }
    
        glm::vec3 GetLeftDirection() const
        {
            return GetRightDirection() * -1.f;
        }
    
        glm::vec3 GetRightDirection() const
        {
            return glm::normalize(glm::cross(front, up));
        }
    
        glm::vec3 GetUpDirection() const
        {
            return up;
        }
    
        glm::vec3 GetDownDirection() const
        {
            return up * -1.f;
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
            glm::vec3 dir = glm::zero<glm::vec3>();
            if (isWKeyHolding)
            {
                dir = GetFrontDirection();
            }
            else if (isSKeyHolding)
            {
                dir = GetBackDirection();
            }
            else if (isAKeyHolding)
            {
                dir = GetLeftDirection();
            }
            else if (isDKeyHolding)
            {
                dir = GetRightDirection();
            }
            if (glm::length(dir) >= epsilon)
            {
                Move(dir, distance);
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
            float yawDegree = yawOffset * sensitivityY;
    
            Rotate(GetRightDirection(), pitchDegree);
            Rotate(GetDownDirection(), yawDegree);
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
    
        // transform
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    
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