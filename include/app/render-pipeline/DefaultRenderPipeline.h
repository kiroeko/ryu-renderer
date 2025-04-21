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
            // init meshes
            lightMeshes.emplace_back(Graphics::Mesh(
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
                    { -0.5f, -0.5f, -0.5f },  // 0
                    { 0.5f, -0.5f, -0.5f },   // 1
                    { 0.5f,  0.5f, -0.5f },   // 2
                    { -0.5f,  0.5f, -0.5f },  // 3
                    // 后面
                    { -0.5f, -0.5f,  0.5f },  // 4
                    { 0.5f, -0.5f,  0.5f },   // 5
                    { 0.5f,  0.5f,  0.5f },   // 6
                    { -0.5f,  0.5f,  0.5f },  // 7
                    // 左面
                    { -0.5f,  0.5f,  0.5f },  // 8
                    { -0.5f,  0.5f, -0.5f },  // 9
                    { -0.5f, -0.5f, -0.5f },  // 10
                    { -0.5f, -0.5f,  0.5f },  // 11
                    // 右面
                    { 0.5f,  0.5f,  0.5f },   // 12
                    { 0.5f,  0.5f, -0.5f },   // 13
                    { 0.5f, -0.5f, -0.5f },   // 14
                    { 0.5f, -0.5f,  0.5f },   // 15
                    // 底面
                    { -0.5f, -0.5f, -0.5f },  // 16
                    { 0.5f, -0.5f, -0.5f },   // 17
                    { 0.5f, -0.5f,  0.5f },   // 18
                    { -0.5f, -0.5f,  0.5f },  // 19
                    // 顶面
                    { -0.5f,  0.5f, -0.5f },  // 20
                    { 0.5f,  0.5f, -0.5f },   // 21
                    { 0.5f,  0.5f,  0.5f },   // 22
                    { -0.5f,  0.5f,  0.5f }   // 23
                }, // Position
                std::vector<std::array<float, 3>>{
                    // 前面 (法线)
                    { 0.0f, 0.0f, -1.0f },   // 0
                    { 0.0f,  0.0f, -1.0f },  // 1
                    { 0.0f,  0.0f, -1.0f },  // 2
                    { 0.0f,  0.0f, -1.0f },  // 3
                    // 后面 (法线)
                    { 0.0f,  0.0f,  1.0f },  // 4
                    { 0.0f,  0.0f,  1.0f },  // 5
                    { 0.0f,  0.0f,  1.0f },  // 6
                    { 0.0f,  0.0f,  1.0f },  // 7
                    // 左面 (法线)
                    { -1.0f,  0.0f,  0.0f },  // 8
                    { -1.0f,  0.0f,  0.0f },  // 9
                    { -1.0f,  0.0f,  0.0f },  // 10
                    { -1.0f,  0.0f,  0.0f },  // 11
                    // 右面 (法线)
                    { 1.0f,  0.0f,  0.0f },  // 12
                    { 1.0f,  0.0f,  0.0f },  // 13
                    { 1.0f,  0.0f,  0.0f },  // 14
                    { 1.0f,  0.0f,  0.0f },  // 15
                    // 底面 (法线)
                    { 0.0f, -1.0f,  0.0f },  // 16
                    { 0.0f, -1.0f,  0.0f },  // 17
                    { 0.0f, -1.0f,  0.0f },  // 18
                    { 0.0f, -1.0f,  0.0f },  // 19
                    // 顶面 (法线)
                    { 0.0f,  1.0f,  0.0f },  // 20
                    { 0.0f,  1.0f,  0.0f },  // 21
                    { 0.0f,  1.0f,  0.0f },  // 22
                    { 0.0f,  1.0f,  0.0f }   // 23
                } // Normal
            ));

            // init camera
            camera = Graphics::Scene::Camera(
                glm::vec3(0.0f, 1.0f, 6.0f),
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

            // init box shader
            boxShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-blinn-phong-light.vert", "res/shaders/3d-blinn-phong-light.frag");

            // init mvp
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

            // Update camear
            camera.OnTick(deltaTimeInS);
            view = camera.GetView();

            // draw light
            lightWorldPos.x = std::cos(glfwGetTime() * 1.2f) * 2.0f;
            lightWorldPos.z = std::sin(glfwGetTime() * 1.2f) * 2.0f;
            modelLight = glm::translate(glm::identity<glm::mat4>(), lightWorldPos);
            modelLight = glm::scale(modelLight, glm::vec3(0.2f));

            lightShader->Use();
            lightShader->SetUniform("model", modelLight);
            lightShader->SetUniform("view", view);
            lightShader->SetUniform("projection", projection);
            lightShader->SetUniform("color", lightColor.x, lightColor.y, lightColor.z);

            for (int i = 0; i < lightMeshes.size(); ++i)
            {
                lightMeshes[i].Draw();
            }

            // rotate box
            constexpr float degreesPerSecond = 60.0f;
            constexpr float rotationSpeed = glm::radians(degreesPerSecond);
            static float totalAngle = 0.0f;
            totalAngle += rotationSpeed * (float)deltaTimeInS;
            glm::quat rotation = glm::angleAxis(
                totalAngle,                                // 旋转角度（弧度）
                glm::vec3(0.0f, 1.0f, 0.0f)                // 旋转轴（Y 轴）
            );
            modelBox = glm::mat4_cast(rotation);

            // caculate normalMatrix
            glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(view * modelBox)));

            // draw box
            boxShader->Use();
            boxShader->SetUniform("model", modelBox);
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);
            boxShader->SetUniform("viewNormalMatrix", viewNormalMatrix);
            boxShader->SetUniform("lightWorldPos", lightWorldPos.x, lightWorldPos.y, lightWorldPos.z);
            boxShader->SetUniform("ambientStrength", 0.2f);
            boxShader->SetUniform("specularStrength", 1.f);
            boxShader->SetUniform("shininess", 128.f);
            boxShader->SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
            auto cp = camera.GetPos();
            boxShader->SetUniform("objectColor", boxColor.x, boxColor.y, boxColor.z);

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

        std::vector<RyuRenderer::Graphics::Mesh> lightMeshes;
        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;

        std::shared_ptr<RyuRenderer::Graphics::Shader> lightShader;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        glm::vec3 lightWorldPos = { 0.0f, 1.2f, 0.0f };
        glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
        glm::vec3 boxColor = { 0.0f, 0.678f, 0.937f };

        glm::mat4 modelLight = glm::identity<glm::mat4>();
        glm::mat4 modelBox = glm::identity<glm::mat4>();
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        Graphics::Scene::Camera camera;
    };
}

#endif