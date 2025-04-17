#ifndef __DEFAULTRENDERPIPELINE_H__
#define __DEFAULTRENDERPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include <vector>

#include "app/App.h"
#include "app/events/WindowEvent.h"
#include "app/render-pipeline/IRenderPipeline.h"
#include "graphics/Frame.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"

namespace RyuRenderer::App::RenderPipeline
{
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
            float cNearPlane = 0.001f,
            float cFarPlane = 1000000.f)
        {
            LookAt(cPos, cFront, cUp);
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

        void LookAt(
            const glm::vec3& cPos,
            const glm::vec3& cFront,
            const glm::vec3& cUp)
        {
            pos = cPos;
            front = glm::normalize(cFront);
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

        glm::vec3 GetFront() const
        {
            return front;
        }

        glm::vec3 GetBack() const
        {
            return front * -1.f;
        }

        glm::vec3 GetLeft() const
        {
            return GetRight() * -1.f;
        }

        glm::vec3 GetRight() const
        {
            return glm::normalize(glm::cross(front, up));
        }

        glm::vec3 GetUp() const
        {
            return up;
        }

        glm::vec3 GetDown() const
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
        float nearPlane = 0.001f;
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
        }

        void tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            model = glm::rotate(glm::identity<glm::mat4>(), (float)glfwGetTime() * glm::radians(60.f), glm::vec3(0.0f, 1.0f, 0.0f));
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
            if (e.Event == Events::WindowEvent::EventType::WINDOW_RESIZE)
            {
                camera.OnWindowResize(e.Width, e.Height);
                projection = camera.GetProjection();
            }
        }

        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;
        RyuRenderer::Graphics::Texture2d boxTexture;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        glm::mat4 model = glm::identity<glm::mat4>();
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();
        PerspectiveCamera camera;
    };
}

#endif