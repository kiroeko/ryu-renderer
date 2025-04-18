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

    class PerspectiveCamera
    {
    public:
        PerspectiveCamera() = default;

        PerspectiveCamera(
            const glm::vec3& cPos,
            const glm::vec3& cFront,
            const glm::vec3& cUp,
            int windowWidth,
            int windowHeight,
            float cVFOV = 60.f,
            float cNearPlane = 0.01f,
            float cFarPlane = 1000000.f)
        {
            pos = cPos;
            RotateTo(cFront, cUp);
            vFOV = cVFOV;
            nearPlane = cNearPlane;
            farPlane = cFarPlane;
            aspectRatio = (float)windowWidth / windowHeight;
        }

        void Move(const glm::vec3& direction, float distance)
        {
            glm::vec3 dir = glm::normalize(direction);
            pos += dir * distance;
        }

        void MoveTo(const glm::vec3& cPos)
        {
            pos = cPos;
        }

        void Rotate(const glm::vec3& rotateAxis, float degree)
        {
            float angle = glm::radians(degree);
            glm::quat rotation = glm::angleAxis(angle, rotateAxis);
            front = rotation * front;
        }

        void RotateTo(
            const glm::vec3& cFront,
            const glm::vec3& cUp)
        {
            front = glm::normalize(cFront);
            up = glm::normalize(cUp);
        }

        void LookAt(
            const glm::vec3& targetPos,
            const glm::vec3& cUp)
        {
            front = targetPos - pos;
            up = glm::normalize(cUp);
        }

        void OnWindowResize(int windowWidth, int windowHeight)
        {
            aspectRatio = (float)windowWidth / windowHeight;
        }

        void SetVFov(float cVFOV)
        {
            vFOV = cVFOV;
            isBasedOnVFOV = true;
        }

        void SetHFov(float cHFOV)
        {
            hFOV = cHFOV;
            isBasedOnVFOV = false;
        }
        
        void SetNearPlane(const float& cNearPlane)
        {
            nearPlane = cNearPlane;
        }

        void SetFarPlane(const float& cFarPlane)
        {
            farPlane = cFarPlane;
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
            return glm::perspective(
                glm::radians(GetVFOV()),
                aspectRatio,
                nearPlane,
                farPlane
            );
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

        float GetAspectRatio() const
        {
            return aspectRatio;
        }

        float GetVFOV() const
        {
            if (isBasedOnVFOV)
            {
                return vFOV;
            }

            float horizontalFovRad = glm::radians(hFOV);
            float verticalFovRad = 2.0f * std::atan(std::tan(horizontalFovRad / 2.0f) / aspectRatio);
            return glm::degrees(verticalFovRad);
        }

        float GetHFOV() const
        {
            if (isBasedOnVFOV)
            {
                float verticalFovRad = glm::radians(vFOV);
                float horizontalFovRad = 2.0f * atan(tan(verticalFovRad / 2.0f) * aspectRatio);
                return glm::degrees(horizontalFovRad);
            }
            
            return hFOV;
        }

        float GetNearPlane() const
        {
            return nearPlane;
        }

        float GetFarPlane() const
        {
            return farPlane;
        }
    private:
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float aspectRatio = 16.f / 9.f;
        bool isBasedOnVFOV = true;
        float vFOV = 60.f;
        float hFOV = 91.513f;
        float nearPlane = 0.01f;
        float farPlane = 1000000.f;
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

            boxShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-unlit-simple.vert", "res/shaders/3d-unlit-simple.frag");
            if (boxShader)
            {
                boxShader->Use();
                boxShader->SetUniform("diffuseTexture", 0);
            }

            // init camera
            camera = PerspectiveCamera(
                glm::vec3(0.0f, 0.0f, 6.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                App::GetInstance().GetWindowWidth(),
                App::GetInstance().GetWindowHeight()
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

            constexpr float speed = 10.f;
            float distance = deltaTimeInS * speed;

            constexpr float epsilon = 1e-6f;
            glm::vec3 dir = glm::zero<glm::vec3>();
            if (isWKeyHolding)
            {
                dir = camera.GetFrontDirection();
            }
            else if (isSKeyHolding)
            {
                dir = camera.GetBackDirection();
            }
            else if (isAKeyHolding)
            {
                dir = camera.GetLeftDirection();
            }
            else if (isDKeyHolding)
            {
                dir = camera.GetRightDirection();
            }
            if (glm::length(dir) >= epsilon)
            {
                camera.Move(dir, distance);
            }

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

            camera.OnWindowResize(e.Width, e.Height);
            projection = camera.GetProjection();
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
            
            camera.Rotate(camera.GetRightDirection(), pitchDegree);
            camera.Rotate(camera.GetDownDirection(), yawDegree);
        }

        void OnKeyEvent(const Events::KeyEvent& e)
        {
            if (e.Action == Events::KeyEvent::ActionType::ACTION_PRESS)
            {
                if (e.Key == Events::KeyEvent::KeyType::KEY_R)
                {
                    camera.MoveTo(glm::vec3(0.0f, 0.0f, 6.0f));
                    camera.LookAt(
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

        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;
        RyuRenderer::Graphics::Texture2d boxTexture;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        glm::mat4 model = glm::identity<glm::mat4>();
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        PerspectiveCamera camera;

        bool isWKeyHolding = false;
        bool isSKeyHolding = false;
        bool isAKeyHolding = false;
        bool isDKeyHolding = false;
    };
}

#endif