#ifndef __APP_H__
#define __APP_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include "common/Singleton.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"
#include "graphics/Frame.h"

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace RyuRenderer::App
{
    class App : public RyuRenderer::Common::Singleton<App>
    {
    public:
        ~App()
        {
            Clear();
        }

        bool Init(std::string name = "", int initWidth = 1920, int initHeight = 1080)
        {
            Clear();

            // init glfw
            glfwInit();

            // create window
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            window = glfwCreateWindow(initWidth, initHeight, name.c_str(), NULL, NULL);
            if (window == nullptr)
            {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }
            glfwMakeContextCurrent(window);

            // init glad
            if (!gladLoadGL(glfwGetProcAddress))
            {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                glfwTerminate();
                window = nullptr;
                return false;
            }

            // other settings
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            windowWidth = initWidth;
            windowHeight = initHeight;
            glfwSetFramebufferSizeCallback(window, onWindowSizeChanged);

            stbi_set_flip_vertically_on_load(true);

            return true;
        }

        void Run()
        {
            if (!window)
            {
                std::cerr << "App initialization incorrect, unable to run." << std::endl;
                return;
            }
            
            initRenderer();

            // main loop
            while (!glfwWindowShouldClose(window))
            {
                // handle input events
                glfwPollEvents();
                processInput(window);

                // clear canvas
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                // render
                renderTick();

                // show render result
                glfwSwapBuffers(window);
            }
        }

        int GetWindowWidth()
        {
            return windowWidth;
        }

        int GetWindowHeight()
        {
            return windowHeight;
        }
    private:
        // 这里我们简单地去接受一下键盘和鼠标的数据，并根据鼠标位置调整 Shader 参数来让绘制图形根据鼠标颜色而不同，
        //     如果要封装得和游戏引擎一样，那我们应该接受所有的输入事件和游戏引擎内部定义的事件，把事件们放到事件队列里，再由我们自己定义的事件类型有关回调逐个处理。
        void processInput(GLFWwindow* window)
        {
            // ESC 退出
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }

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
            frameTextures[0] = RyuRenderer::Graphics::Texture2d(GL_RGB, 0, windowWidth, windowHeight);
            frameTextures[1] = RyuRenderer::Graphics::Texture2d(GL_RGB, 0, windowWidth, windowHeight);

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

        void Clear()
        {
            if (window)
                glfwTerminate();

            windowWidth = 0;
            windowHeight = 0;
            window = nullptr;
        }

        static void onWindowSizeChanged(GLFWwindow* window, int width, int height)
        {
            if (!window)
                return;
            if (!App::GetInstance().window)
                return;
            if (window != App::GetInstance().window)
                return;

            glViewport(0, 0, width, height);
            App::GetInstance().windowWidth = width;
            App::GetInstance().windowHeight = height;
        }

        GLFWwindow* window = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;

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