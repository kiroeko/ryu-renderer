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

            // init mvp
            view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 6.0f), // 摄像机位置
                glm::vec3(0.0f, 0.0f, 0.0f), // 看向的目标点
                glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向，设为 Y 轴正方向
            );
            projection = glm::perspective(glm::radians(60.0f), (float)App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(), 0.001f, 1000000.0f);

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnWindowResize);
        }

        void tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            model = glm::rotate(glm::identity<glm::mat4>(), (float)glfwGetTime() * glm::radians(60.f), glm::vec3(0.0f, 1.0f, 0.0f));
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
                projection = glm::perspective(glm::radians(10.0f), (float)App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(), 0.001f, 1000000.0f);
            }
        }

        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;
        RyuRenderer::Graphics::Texture2d boxTexture;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        glm::mat4 model = glm::identity<glm::mat4>();
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();
    };
}

#endif