#ifndef __DEFAULTPIPELINE_H__
#define __DEFAULTPIPELINE_H__

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
                    0, 3, 7,
                    7, 4, 0,
                    // 右面
                    1, 5, 6,
                    6, 2, 1,
                    // 底面
                    0, 1, 5,
                    5, 4, 0,
                    // 顶面
                    3, 2, 6,
                    6, 7, 3
                }, // indexes
                std::vector<std::array<float, 3>>{
                    {  -0.5f, -0.5f, -0.5f },
                    { 0.5f, -0.5f, -0.5f },
                    { 0.5f,  0.5f, -0.5f },
                    { -0.5f,  0.5f, -0.5f },
                    { -0.5f, -0.5f,  0.5f },
                    { 0.5f, -0.5f, 0.5f },
                    { 0.5f, 0.5f, 0.5f },
                    { -0.5f,  0.5f,  0.5f },
                } // Position
            ));

            // init camera
            camera = Graphics::Scene::Camera(
                glm::vec3(0.0f, 0.0f, 6.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                0.01f,
                1000000.f,
                60.f,
                (float)App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(),
                true
            );

            // init light shader
            lightShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-basic-color.vert", "res/shaders/3d-basic-color.frag");
            if (lightShader)
            {
                lightShader->Use();
                lightShader->SetUniform("color", lightColor.x, lightColor.y, lightColor.z);
            }
;
            glm::vec3 lightPos(1.2f, 1.0f, -2.0f);
            modelLight = glm::translate(modelLight, lightPos);
            modelLight = glm::scale(modelLight, glm::vec3(0.2f));

            // init box shader
            boxShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-light-test-0.vert", "res/shaders/3d-light-test-0.frag");
            if (boxShader)
            {
                boxShader->Use();
                boxShader->SetUniform("objectColor", 1.0f, 0.5f, 0.31f);
                boxShader->SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
            }

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
            view = camera.GetView();

            constexpr float degreesPerSecond = 60.0f;
            constexpr float rotationSpeed = glm::radians(degreesPerSecond);
            static float totalAngle = 0.0f;
            totalAngle += rotationSpeed * (float)deltaTimeInS;

            glm::quat rotation = glm::angleAxis(
                totalAngle,                                // 旋转角度（弧度）
                glm::vec3(0.0f, 1.0f, 0.0f)                // 旋转轴（Y 轴）
            );
            modelBox = glm::mat4_cast(rotation);

            boxShader->Use();
            boxShader->SetUniform("model", modelBox);
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);

            for (int i = 0; i < boxMeshes.size(); ++i)
            {
                boxMeshes[i].Draw();
            }

            lightShader->Use();
            lightShader->SetUniform("model", modelLight);
            lightShader->SetUniform("view", view);
            lightShader->SetUniform("projection", projection);

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
        std::shared_ptr<RyuRenderer::Graphics::Shader> lightShader;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;
        glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };

        glm::mat4 modelLight = glm::identity<glm::mat4>();
        glm::mat4 modelBox = glm::identity<glm::mat4>();

        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        Graphics::Scene::Camera camera;
    };
}

#endif