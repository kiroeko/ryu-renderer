#ifndef __GUASSIANBLURPIPELINE_H__
#define __GUASSIANBLURPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

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
    class GuassianBlurPipeline : public IRenderPipeline
    {
    public:
    GuassianBlurPipeline() = default;

        void init() override
        {
            initRenderer();

            App::GetInstance().EventPublisher.RegisterHandler(this, &GuassianBlurPipeline::OnWindowResize);
        }

        void tick() override
        {
            renderTick();
        }
    private:
        void initRenderer()
        {
            initMainScene();

            initFullScreenQuadMesh();

            initFrames();

            initOtherSettings();
        }

        void initMainScene()
        {
            // 场景本身，这里恰好是带纹理图案的矩形
            sceneMesh = RyuRenderer::Graphics::Mesh(
                std::vector<GLuint>{0, 1, 2, 0, 2, 3}, // indexes
                std::vector<std::array<float, 2>>{{ -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f }},// Position
                std::vector<std::array<float, 2>>{{ 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }} // TexCoord
            );

            // 加载场景贴图
            sceneTexture = RyuRenderer::Graphics::Texture2d("res/textures/test.jpg", 0);

            simpleShader = shaderManager.Create("res/shaders/2d-texture.vert", "res/shaders/2d-texture.frag");
            if (simpleShader)
            {
                simpleShader->Use();
                simpleShader->SetUniform("mainTexture", 0);
            }
        }

        void initFullScreenQuadMesh()
        {
            fullScreenQuadMesh = RyuRenderer::Graphics::Mesh(
                std::vector<GLuint>{0, 1, 2, 0, 2, 3}, // indexes
                std::vector<std::array<float, 2>>{{ -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f }},// Position
                std::vector<std::array<float, 2>>{{ 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }} // TexCoord
            );
        }

        void initOtherSettings()
        {
            // 加载 高斯模糊的 shader
            gaussianBlurShader = shaderManager.Create("res/shaders/gaussian-blur.vert", "res/shaders/gaussian-blur.frag");
            if (gaussianBlurShader)
            {
                gaussianBlurShader->Use();
                gaussianBlurShader->SetUniform("mainTexture", 0);
            }
        }

        void initFrames()
        {
            auto w = RyuRenderer::App::App::GetInstance().GetWindowWidth();
            auto h = RyuRenderer::App::App::GetInstance().GetWindowHeight();

            frameTextures[0] = RyuRenderer::Graphics::Texture2d(GL_RGB, 0, w, h);
            frameTextures[1] = RyuRenderer::Graphics::Texture2d(GL_RGB, 0, w, h);

            frames[0] = RyuRenderer::Graphics::Frame(&frameTextures[0]);
            frames[1] = RyuRenderer::Graphics::Frame(&frameTextures[1]);
        }

        // 这里我们简单合批渲染一下由不同材质参数的多个物件，
       //     实际上可以使用复杂的场景物体预剔除、LOD以及合批的策略，优化这一步的性能，但其实对于几个场景物体，也没啥优化空间。
        void renderTick()
        {
            // 先绑定 fbo0
            frames[0].Use();

            // 渲染场景本身到 fbo 里面
            sceneTexture.Use();
            simpleShader->Use();
            sceneMesh.Draw();

            // 进行高斯模糊，水平+垂直共迭代 5 次
            gaussianBlurShader->Use();
            bool horizontal = true, firstIteration = true;
            constexpr int amount = 30; // 模糊迭代次数
            for (unsigned int i = 0; i < amount; ++i)
            {
                frames[horizontal].Use();
                if (firstIteration)
                    sceneTexture.Use();
                else
                    frameTextures[!horizontal].Use();
                gaussianBlurShader->SetUniform("isHorizontal", horizontal);

                // 渲染全屏四边形到 fbo 里
                fullScreenQuadMesh.Draw();

                // 状态管理
                horizontal = !horizontal;
                if (firstIteration)
                    firstIteration = false;
            }
            RyuRenderer::Graphics::Frame::Unuse();

            // 把最后一次渲染出的 fbo 纹理作为结果输出到 OpenGl 画布上
            frameTextures[!horizontal].Use();
            simpleShader->Use();
            fullScreenQuadMesh.Draw();
        }

        void OnWindowResize(const Events::WindowEvent& e)
        {
            if (e.Event == Events::WindowEvent::EventType::WINDOW_RESIZE)
            {
                initFrames();
            }
        }

        // tmp
        ShaderManager shaderManager;

        RyuRenderer::Graphics::Mesh sceneMesh;
        RyuRenderer::Graphics::Texture2d sceneTexture;
        std::shared_ptr<RyuRenderer::Graphics::Shader> simpleShader;

        RyuRenderer::Graphics::Mesh fullScreenQuadMesh;

        RyuRenderer::Graphics::Frame frames[2] = {};
        RyuRenderer::Graphics::Texture2d frameTextures[2] = {};

        std::shared_ptr<RyuRenderer::Graphics::Shader> gaussianBlurShader;
    };
}

#endif