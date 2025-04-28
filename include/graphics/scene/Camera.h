#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"

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
        );
    
        Camera(
            const glm::vec3& position,
            const glm::vec3& frontDir,
            const glm::vec3& upDir,
            float nearPlaneDis = 0.01f,
            float farPlaneDis = 1000000.f,
            float vfov = 60.f,
            float hfov = 60.f
        );
    
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
        );
    
        void SetNearPlane(const float& nearPlaneDis);
    
        void SetFarPlane(const float& farPlaneDis);
    
        void SetIsPerspective(bool isPerspect);
    
        void SetVFov(float vfov);
    
        void SetHFov(float hfov);
    
        void SetOrthoWidth(float orthoW);
    
        void SetOrthoHeight(float orthoH);
    
        void SetIsAspectRatioPriority(bool isAspectPriority);
    
        void SetAspectRatio(float aspect);
    
        void SetIsVFOVPriority(bool isVPriority);
    
        void SetIsOrthoWidthPriority(bool isOrthoWPriority);
    
        glm::mat4 GetView() const;
    
        glm::mat4 GetProjection() const;
    
        float GetNearPlane() const;
    
        float GetFarPlane() const;
    
        bool GetIsPerspective() const;
    
        float GetVFov() const;
    
        float GetHFov() const;
    
        float GetOrthoWidth() const;
    
        float GetOrthoHeight() const;
    
        bool GetIsAspectRatioPriority();
    
        float GetAspectRatio() const;
    
        bool GetIsVFOVPriority();
    
        bool GetIsOrthoWidthPriority();
    
        bool IsAspectVaild();
    
        // Controll
        void OnTick(double deltaTimeInS);
    
        void OnMouseMove(const App::Events::MouseEvent& e);
    
        void OnKeyEvent(const App::Events::KeyEvent& e);

        Transform Transformer;

        float MoveSpeed = 10.f;
        float PitchSensitivity = 0.02f;
        float YawSensitivity = 0.05f;
    private:
        static float GetHFOV(float vfovDegree, float aspectRatio);
    
        static float GetVFOV(float hfovDegree, float aspectRatio);
    
        void MaintenForAspectRatio();
    
        float nearPlane = 0.f;
        float farPlane = 0.f;
    
        bool isPerspective = true;
    
        float vFOV = 0.f;
        float hFOV = 0.f;
    
        float orthoWidth = 0.f;
        float orthoHeight = 0.f;
    
        float aspectRatio = 0.f;
        // If true, when aspectRatio is changed, vFOV / hFOV / orthoWidth / orthoHeight will be automatically adjusted to match the aspectRatio value;
        //     otherwise, aspectRatio will be automatically adjusted to match the values of vFOV / hFOV / orthoWidth / orthoHeight.
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