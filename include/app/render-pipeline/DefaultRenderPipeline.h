#ifndef __DEFAULTRENDERPIPELINE_H__
#define __DEFAULTRENDERPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include <vector>

#include "app/App.h"
#include "app/events/WindowEvent.h"
#include "app/events/MouseEvent.h"
#include "app/render-pipeline/IRenderPipeline.h"
#include "graphics/Frame.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"

namespace RyuRenderer::App::RenderPipeline
{
    class World
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

        void OnMouseMove(const Events::MouseEvent& e)
        {
            if (e.Event != Events::MouseEvent::EventType::MOUSE_MOVE)
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

        void OnKeyEvent(const Events::KeyEvent& e)
        {
            if (e.Action == Events::KeyEvent::ActionType::ACTION_PRESS)
            {
                if (e.Key == Events::KeyEvent::KeyType::KEY_R)
                {
                    MoveTo(glm::vec3(0.0f, 0.0f, 6.0f));
                    LookAt(
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                    );
                }

                if (e.Key == Events::KeyEvent::KeyType::KEY_W)
                {
                    isWKeyHolding = true;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_S)
                {
                    isSKeyHolding = true;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_A)
                {
                    isAKeyHolding = true;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_D)
                {
                    isDKeyHolding = true;
                }
            }
            else if (e.Action == Events::KeyEvent::ActionType::ACTION_RELEASE)
            {
                if (e.Key == Events::KeyEvent::KeyType::KEY_W)
                {
                    isWKeyHolding = false;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_S)
                {
                    isSKeyHolding = false;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_A)
                {
                    isAKeyHolding = false;
                }
                else if (e.Key == Events::KeyEvent::KeyType::KEY_D)
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

    class DefaultRenderPipeline : public IRenderPipeline
    {
    public:
        DefaultRenderPipeline() = default;

        void init() override
        {
            // Box
            boxMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // 前面
                    0, 1, 2,
                    2, 3, 0,
                    // 后面
                    4, 5, 6,
                    6, 7, 4,
                    // 左面
                    8, 9, 10,
                    10, 11, 8,
                    // 右面
                    12, 13, 14,
                    14, 15, 12,
                    // 底面
                    16, 17, 18,
                    18, 19, 16,
                    // 顶面
                    20, 21, 22,
                    22, 23, 20
                }, // indexes
                std::vector<std::array<float, 3>>{
                    // 前面
                    { -1.0f, -1.0f, -1.0f },
                    { 1.0f, -1.0f, -1.0f },
                    { 1.0f, 1.0f, -1.0f },
                    { -1.0f, 1.0f, -1.0f },
                    // 后面
                    { -1.0f, -1.0f, 1.0f },
                    { 1.0f, -1.0f, 1.0f },
                    { 1.0f, 1.0f, 1.0f },
                    { -1.0f, 1.0f, 1.0f },
                    // 左面
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, 1.0f, -1.0f },
                    { -1.0f, 1.0f, 1.0f },
                    { -1.0f, -1.0f, 1.0f },
                    // 右面
                    { 1.0f, -1.0f, -1.0f },
                    { 1.0f, 1.0f, -1.0f },
                    { 1.0f, 1.0f, 1.0f },
                    { 1.0f, -1.0f, 1.0f },
                    // 底面
                    { -1.0f, -1.0f, -1.0f },
                    { 1.0f, -1.0f, -1.0f },
                    { 1.0f, -1.0f, 1.0f },
                    { -1.0f, -1.0f, 1.0f },
                    // 顶面
                    { -1.0f, 1.0f, -1.0f },
                    { 1.0f, 1.0f, -1.0f },
                    { 1.0f, 1.0f, 1.0f },
                    { -1.0f, 1.0f, 1.0f },
                },// Position
                std::vector<std::array<float, 2>>{
                    // 前面
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // 后面
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // 左面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 右面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 底面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 顶面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f }
                } // TexCoord
            ));

            boxTexture = Graphics::Texture2d("res/textures/box.jpg", 0);
            boxTexture.Use();

            boxShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-basic-texture.vert", "res/shaders/3d-basic-texture.frag");
            if (boxShader)
            {
                boxShader->Use();
                boxShader->SetUniform("diffuseTexture", 0);
            }

            // init camera
            camera = Camera(
                glm::vec3(0.0f, 0.0f, 6.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                0.01f,
                1000000.f,
                60.f,
                (float) App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(),
                true
            );

            view = camera.GetView();
            projection = camera.GetProjection();

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnKeyEvent);
        }

        void tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            camera.OnTick(deltaTimeInS);

            constexpr float degreesPerSecond = 60.0f;
            constexpr float rotationSpeed = glm::radians(degreesPerSecond);
            static float totalAngle = 0.0f;
            totalAngle += rotationSpeed * (float)deltaTimeInS;

            glm::quat rotation = glm::angleAxis(
                totalAngle,                                // 旋转角度（弧度）
                glm::vec3(0.0f, 1.0f, 0.0f)                // 旋转轴（Y 轴）
            );
            model = glm::mat4_cast(rotation);
            view = camera.GetView();

            boxShader->SetUniform("model", model);
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);

            for (int i = 0; i < boxMeshes.size(); ++i)
            {
                boxMeshes[i].Draw();
            }
        }
    private:
        void OnWindowResize(const Events::WindowEvent& e)
        {
            if (e.Event != Events::WindowEvent::EventType::WINDOW_RESIZE)
                return;

            camera.SetAspectRatio((float)e.Width / e.Height);
            projection = camera.GetProjection();
        }

        void OnMouseMove(const Events::MouseEvent& e)
        {
            camera.OnMouseMove(e);
        }

        void OnKeyEvent(const Events::KeyEvent& e)
        {
            camera.OnKeyEvent(e);
        }

        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;
        RyuRenderer::Graphics::Texture2d boxTexture;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        glm::mat4 model = glm::identity<glm::mat4>();
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        Camera camera;
    };
}

#endif