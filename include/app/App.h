#ifndef __APP_H__
#define __APP_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include "common/Singleton.h"
#include "graphics/Shader.h"

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
            
            // ��ʾ�����ֹ���ڱ仯
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
        // �������Ǽ򵥵�ȥ����һ�¼��̺��������ݣ����������λ�õ��� Shader �������û���ͼ�θ��������ɫ����ͬ��
        //     ���Ҫ��װ�ú���Ϸ����һ����������Ӧ�ý������е������¼�����Ϸ�����ڲ�������¼������¼��Ƿŵ��¼���������������Լ�������¼������йػص��������
        void processInput(GLFWwindow* window)
        {
            // ESC �˳�
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }

        // �������Ǽ��ֶ���дһ������� mesh �������йصı任��������ݣ�
        //     ʵ���� mesh һ���� fbx �ļ����룬
        //     �任����һ����ͨ����Ϸ�����༭�����õ���ֵ��
        void initScene()
        {
            initMainScene();

            initQuad();

            initShader();

            initFBO();
        }

        void initMainScene()
        {
            // ������������ǡ���Ǵ�����ͼ���ľ���
            // �� mesh �� VAO
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

            // ���س�����ͼ
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &SceneTexture);
            glBindTexture(GL_TEXTURE_2D, SceneTexture);
            // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // ���ز���������
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
            // ��Ⱦģ���õ�ģ�ͣ�������һ��������ģ�����Ļ��Сһ�µľ���
            // �� mesh �� VAO
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
            // ���� �����õ� shader
            simpleShader = OGLRenderer::Graphics::Shader("res/shaders/simple.vs", "res/shaders/simple.fs");
            simpleShader.Use();
            // 0 ���� GL_TEXTURE0
            simpleShader.SetUniformWithInt("mainTexture", 0);

            // ���� ��˹ģ���� shader
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

        // �������Ǽ򵥺�����Ⱦһ���ɲ�ͬ���ʲ����Ķ�������
        //     ʵ���Ͽ���ʹ�ø��ӵĳ�������Ԥ�޳���LOD�Լ������Ĳ��ԣ��Ż���һ�������ܣ�����ʵ���ڼ����������壬Ҳûɶ�Ż��ռ䡣
        void renderScene()
        {
            // �Ȱ� fbo0
            glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);

            // ��Ⱦ�������� fbo ����
            simpleShader.Use();
            glBindTexture(GL_TEXTURE_2D, SceneTexture);
            glBindVertexArray(SceneVAO);
            glDrawElements(GL_TRIANGLES, SceneElementCount, GL_UNSIGNED_INT, 0);

            // ���и�˹ģ����ˮƽ+��ֱ������ 5 ��
            blurShader.Use();
            bool horizontal = true, firstIteration = true;
            constexpr int amount = 10; // ģ����������
            for (unsigned int i = 0; i < amount; ++i)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo[horizontal]);
                glBindTexture(GL_TEXTURE_2D, firstIteration ? SceneTexture : fboTextures[!horizontal]);
                blurShader.SetUniformWithInt("isHorizontal", (int)horizontal);

                // ��Ⱦȫ���ı��ε� fbo ��
                glBindVertexArray(QuadVAO);
                glDrawElements(GL_TRIANGLES, QuadElementCount, GL_UNSIGNED_INT, 0);

                // ״̬����
                horizontal = !horizontal;
                if (firstIteration)
                    firstIteration = false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // �����һ����Ⱦ���� fbo ������Ϊ�������� OpenGl ������
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