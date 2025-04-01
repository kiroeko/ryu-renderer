#ifndef __APP_H__
#define __APP_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include "common/Singleton.h"
#include "graphics/Shader.h"

#include "graphics/ShaderManager.h"

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace OGLRenderer::App
{
    class App : public OGLRenderer::Common::Singleton<App>
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

            // create glfw window
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            // 演示代码禁止窗口变化
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            
            window = glfwCreateWindow(initWidth, initHeight, name.c_str(), NULL, NULL);
            if (window == nullptr)
            {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }
            glfwMakeContextCurrent(window);

            // load glad
            if (!gladLoadGL(glfwGetProcAddress))
            {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                glfwTerminate();
                window = nullptr;
                return false;
            }

            // other gl and window settings
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            windowWidth = initWidth;
            windowHeight = initHeight;
            glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

            return true;
        }

        void Run()
        {
            if (!window)
            {
                std::cerr << "App initialization incorrect, unable to run." << std::endl;
                return;
            }
            
            initScene();

            // main loop
            while (!glfwWindowShouldClose(window))
            {
                // handle input events
                glfwPollEvents();
                processInput(window);

                // clear canvas
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // render
                renderScene();

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

        // 这里我们简单手动填写一下所需的 mesh 和它们有关的变换矩阵的数据，
        //     实际上 mesh 一般由 fbx 文件读入，
        //     变换矩阵一般是通过游戏场景编辑器设置的数值。
        void initScene()
        {
            initMainScene();

            initQuad();

            initShader();

            initFBO();
        }

        void initMainScene()
        {
            // 场景本身，这里恰好是带纹理图案的矩形
            // 绑定 mesh 到 VAO
            GLfloat vertices[] = {
                // Position   // TexCoord
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f
            };
            GLuint indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            // VAOs
            glGenVertexArrays(1, &SceneVAO);
            // VBOs
            GLuint VBO = 0;
            glGenBuffers(1, &VBO);
            // IBOs
            GLuint IBO;
            glGenBuffers(1, &IBO);

            // VAO Binding
            glBindVertexArray(SceneVAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
            SceneElementCount = std::size(indices);

            glBindVertexArray(0);

            // 加载场景贴图
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &SceneTexture);
            glBindTexture(GL_TEXTURE_2D, SceneTexture);
            // 为当前绑定的纹理对象设置环绕、过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 加载并生成纹理
            int textureWidth = 0, textureHeight = 0, textureNRChannels = 0;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* textureData = stbi_load("res/textures/test.jpg", &textureWidth, &textureHeight, &textureNRChannels, 0);
            if (textureData)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
            }
            else
            {
                std::cerr << "Failed to load texture" << std::endl;
            }
            stbi_image_free(textureData);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void initQuad()
        {
            // 渲染模糊用的模型，这里是一个带纹理的，和屏幕大小一致的矩形
            // 绑定 mesh 到 VAO
            GLfloat vertices[] = {
                // Position   // TexCoord
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f
            };
            GLuint indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            // VAOs
            glGenVertexArrays(1, &QuadVAO);
            // VBOs
            GLuint VBO = 0;
            glGenBuffers(1, &VBO);
            // IBOs
            GLuint IBO;
            glGenBuffers(1, &IBO);

            // VAO Binding
            glBindVertexArray(QuadVAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
            QuadElementCount = std::size(indices);

            glBindVertexArray(0);
        }

        void initShader()
        {
            // 加载 场景用的 shader
            simpleShader = OGLRenderer::Graphics::Shader("res/shaders/simple.vs", "res/shaders/simple.fs");
            simpleShader.Use();
            // 0 代表 GL_TEXTURE0
            simpleShader.SetUniformWithInt("mainTexture", 0);

            // 加载 高斯模糊的 shader
            blurShader = OGLRenderer::Graphics::Shader("res/shaders/blur.vs", "res/shaders/blur.fs");
            blurShader.Use();
            blurShader.SetUniformWithInt("mainTexture", 0);
        }

        void initFBO()
        {
            glGenFramebuffers(2, fbo);
            glGenTextures(2, fboTextures);

            for (int i = 0; i < 2; ++i) {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
                glBindTexture(GL_TEXTURE_2D, fboTextures[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextures[i], 0);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    std::cerr << "frame buffer status error." << std::endl;
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 这里我们简单合批渲染一下由不同材质参数的多个物件，
        //     实际上可以使用复杂的场景物体预剔除、LOD以及合批的策略，优化这一步的性能，但其实对于几个场景物体，也没啥优化空间。
        void renderScene()
        {
            // 先绑定 fbo0
            glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);

            // 渲染场景本身到 fbo 里面
            simpleShader.Use();
            glBindTexture(GL_TEXTURE_2D, SceneTexture);
            glBindVertexArray(SceneVAO);
            glDrawElements(GL_TRIANGLES, SceneElementCount, GL_UNSIGNED_INT, 0);

            // 进行高斯模糊，水平+垂直共迭代 5 次
            blurShader.Use();
            bool horizontal = true, firstIteration = true;
            constexpr int amount = 30; // 模糊迭代次数
            for (unsigned int i = 0; i < amount; ++i)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo[horizontal]);
                glBindTexture(GL_TEXTURE_2D, firstIteration ? SceneTexture : fboTextures[!horizontal]);
                blurShader.SetUniformWithInt("isHorizontal", (int)horizontal);

                // 渲染全屏四边形到 fbo 里
                glBindVertexArray(QuadVAO);
                glDrawElements(GL_TRIANGLES, QuadElementCount, GL_UNSIGNED_INT, 0);

                // 状态管理
                horizontal = !horizontal;
                if (firstIteration)
                    firstIteration = false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 把最后一次渲染出的 fbo 纹理作为结果输出到 OpenGl 画布上
            simpleShader.Use();
            glBindTexture(GL_TEXTURE_2D, fboTextures[!horizontal]);
            glBindVertexArray(QuadVAO);
            glDrawElements(GL_TRIANGLES, QuadElementCount, GL_UNSIGNED_INT, 0);
        }

        void Clear()
        {
            if (window)
                glfwTerminate();

            windowWidth = 0;
            windowHeight = 0;
            window = nullptr;
        }

        static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
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
        GLuint SceneVAO = 0;
        size_t SceneElementCount = 0;
        GLuint SceneTexture = 0;

        GLuint QuadVAO = 0;
        size_t QuadElementCount = 0;

        OGLRenderer::Graphics::Shader simpleShader;
        OGLRenderer::Graphics::Shader blurShader;

        GLuint fbo[2] = { 0 };
        GLuint fboTextures[2] = { 0 };
    };
}

#endif