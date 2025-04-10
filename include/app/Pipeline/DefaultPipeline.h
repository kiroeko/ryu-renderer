#ifndef __DEFAULTPIPELINE_H__
#define __DEFAULTPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include "app/App.h"
#include "app/Pipeline/IPipeline.h"
#include "graphics/Frame.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"

namespace RyuRenderer::App::Pipeline
{
    class DefaultPipeline : public IPipeline
    {
    public:
        DefaultPipeline() = default;

        void init() override
        {
            initRenderer();
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
            // ������������ǡ���Ǵ�����ͼ���ľ���
            sceneMesh = RyuRenderer::Graphics::Mesh(
                std::vector<GLuint>{0, 1, 2, 0, 2, 3}, // indexes
                std::vector<std::array<float, 2>>{{ -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f }},// Position
                std::vector<std::array<float, 2>>{{ 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }} // TexCoord
            );

            // ���س�����ͼ
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
            // ���� ��˹ģ���� shader
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

        // �������Ǽ򵥺�����Ⱦһ���ɲ�ͬ���ʲ����Ķ�������
       //     ʵ���Ͽ���ʹ�ø��ӵĳ�������Ԥ�޳���LOD�Լ������Ĳ��ԣ��Ż���һ�������ܣ�����ʵ���ڼ����������壬Ҳûɶ�Ż��ռ䡣
        void renderTick()
        {
            // �Ȱ� fbo0
            frames[0].Use();

            // ��Ⱦ�������� fbo ����
            sceneTexture.Use();
            simpleShader->Use();
            sceneMesh.Draw();

            // ���и�˹ģ����ˮƽ+��ֱ������ 5 ��
            gaussianBlurShader->Use();
            bool horizontal = true, firstIteration = true;
            constexpr int amount = 30; // ģ����������
            for (unsigned int i = 0; i < amount; ++i)
            {
                frames[horizontal].Use();
                if (firstIteration)
                    sceneTexture.Use();
                else
                    frameTextures[!horizontal].Use();
                gaussianBlurShader->SetUniform("isHorizontal", horizontal);

                // ��Ⱦȫ���ı��ε� fbo ��
                fullScreenQuadMesh.Draw();

                // ״̬����
                horizontal = !horizontal;
                if (firstIteration)
                    firstIteration = false;
            }
            RyuRenderer::Graphics::Frame::Unuse();

            // �����һ����Ⱦ���� fbo ������Ϊ�������� OpenGl ������
            frameTextures[!horizontal].Use();
            simpleShader->Use();
            fullScreenQuadMesh.Draw();
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