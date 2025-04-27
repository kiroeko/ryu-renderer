#ifndef __TEXTUREBOX3DPIPELINE_H__
#define __TEXTUREBOX3DPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include <vector>

#include "app/App.h"
#include "app/events/WindowEvent.h"
#include "app/events/MouseEvent.h"
#include "app/events/KeyEvent.h"
#include "app/render-pipeline/IRenderPipeline.h"
#include "graphics/Frame.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"
#include "graphics/scene/Camera.h"

namespace RyuRenderer::App::RenderPipeline
{
    class TextureBox3dPipeline : public IRenderPipeline
    {
    public:
        TextureBox3dPipeline() = default;

        void Init() override
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
            camera = Graphics::Scene::Camera(
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
            App::GetInstance().EventPublisher.RegisterHandler(this, &TextureBox3dPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &TextureBox3dPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &TextureBox3dPipeline::OnKeyEvent);
        }

        void Tick(double deltaTimeInS) override
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

        Graphics::Scene::Camera camera;
    };
}

#endif