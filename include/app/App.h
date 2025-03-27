#ifndef __APP_H__
#define __APP_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

            // �������
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPosCallback(window, mouseCallback);

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
            ClearEx();

            // �� mesh �� VAO
            // data
            GLfloat vertices[] = {
                // Position
                 -10.0f, 10.0f, 0.0f,
                 -10.0f, 2.0f,  0.0f,
                 -8.0f,  2.0f,  0.0f,
                 -8.0f,  10.0f, 0.0f,
                 -8.0f,  8.0f,  0.0f,
                 -2.0f,  10.0f, 0.0f,
                 -2.0f,  8.0f,  0.0f
            };
            GLuint indices[] = {
                0, 1, 2,
                0, 2, 3,
                3, 4, 6,
                3, 5, 6
            };

            // VAOs
            glGenVertexArrays(1, &VAO);
            // VBOs
            GLuint VBO = 0;
            glGenBuffers(1, &VBO);
            // IBOs
            GLuint IBO;
            glGenBuffers(1, &IBO);

            // VAO Binding
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
            elementCount = std::size(indices);

            // ����4������� model ����
            // ��һ��Ԫ�أ�ԭ�ز���
            glm::mat4 m1 = glm::identity<glm::mat4>();
            models.push_back(m1);

            // �ڶ���Ԫ�أ��� z �Ḻ���򣨵ѿ�����������ϵ����ԭ��ת 90 �ȣ�Ҳ������ x-y ƽ����ԭ��˳ʱ��ת 90 �ȡ�
            glm::mat4 m2 = glm::identity<glm::mat4>();
            m2 = glm::rotate(m2, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m2);

            // ������Ԫ�أ��� z �Ḻ���򣨵ѿ�����������ϵ����ԭ��ת 180 �ȣ�Ҳ������ x-y ƽ����ԭ��˳ʱ��ת 180 �ȡ�
            glm::mat4 m3 = glm::identity<glm::mat4>();
            m3 = glm::rotate(m3, glm::radians(180.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m3);

            // ���ĸ�Ԫ�أ��� z �Ḻ���򣨵ѿ�����������ϵ����ԭ��ת 270 �ȣ�Ҳ������ x-y ƽ����ԭ��˳ʱ��ת 270 �ȡ�
            glm::mat4 m4 = glm::identity<glm::mat4>();
            m4 = glm::rotate(m4, glm::radians(270.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m4);

            // ���� view ����������ͷ��λ�úͷ���ľ��󣩣����������ͷ�� z ����������һ����λ������ z �Ḻ������ȷ�� z ֵΪ 0 ���������Լ�ǰ�档
            view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 1.0f), // �����λ��
                glm::vec3(0.0f, 0.0f, 0.0f), // �����Ŀ���
                glm::vec3(0.0f, 1.0f, 0.0f)  // �Ϸ�����Ϊ Y ��������
            );

            // �����������ͶӰ������Ϊ����
            UpdateProjection();

            // ���� shader
            s = OGLRenderer::Graphics::Shader("res/shaders/base.vs", "res/shaders/base.fs");
            s.Use();
            // �̶��ľ������
            s.SetUniformWithMatrixFloat("uView", view);

            // ��������� shader ���Բ����������� RGBA ��������ɫ��Ĭ�ϸ�����ɫ
            for (int i = 0; i < 4; ++i)
            {
                meshUniformParams.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
            }
        }

        void UpdateProjection()
        {
            // ��Ļ��С�����仯ʱ��ͶӰ���󱣳ֿ�߱�һ���Է�ֹ����
            float aspectRatio = 0.f;
            if (windowHeight != 0)
                aspectRatio = (float)windowWidth / (float)windowHeight;
            // �������ŵ�ϵ��
            float orthoWidth = 100.0f;
            float orthoHeight = orthoWidth / aspectRatio;

            float nearp = 0.1f;
            float farp = 1000.0f;
            projection = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, nearp, farp);
        }

        // �������Ǽ򵥺�����Ⱦһ���ɲ�ͬ���ʲ����Ķ�������
        //     ʵ���Ͽ���ʹ�ø��ӵĳ�������Ԥ�޳���LOD�Լ������Ĳ��ԣ��Ż���һ�������ܣ�����ʵ���ڼ����������壬Ҳûɶ�Ż��ռ䡣
        void renderScene()
        {
            s.SetUniformWithMatrixFloat("uProjection", projection);
            // �� 4 ����Ļ����
            for (int i = 0; i < 4; ++i)
            {
                s.SetUniformWithMatrixFloat("uModel", models[i]);
                s.SetUniformWithFloat("uColor", meshUniformParams[i].r, meshUniformParams[i].g, meshUniformParams[i].b, meshUniformParams[i].a);
                glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
            }
        }

        void Clear()
        {
            if (window)
                glfwTerminate();

            windowWidth = 0;
            windowHeight = 0;
            window = nullptr;

            ClearEx();
        }

        void ClearEx()
        {
            VAO = 0;
            elementCount = 0;
            models.clear();
            view = glm::mat4();
            projection = glm::mat4();
            s = OGLRenderer::Graphics::Shader();
            meshUniformParams.clear();
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
            App::GetInstance().UpdateProjection();
        }

        static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
        {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);
            float xoffset = App::GetInstance().windowWidth / 2 - xpos;
            float yoffset = App::GetInstance().windowHeight / 2 - ypos;

            if (App::GetInstance().meshUniformParams.size() != 4)
                return;

            if (xoffset == 0 && yoffset == 0)
            {
                return;
            }

            int quadrant = getQuadrant(xoffset, yoffset);
            if (quadrant == -1)
            {
                for (int i = 0; i < App::GetInstance().meshUniformParams.size(); ++i)
                {
                    App::GetInstance().meshUniformParams[i] = glm::vec4(0.f, 0.f, 1.f, 1.f);
                }
                return;
            }

            for (int i = 0; i < App::GetInstance().meshUniformParams.size(); ++i)
            {
                if (i + 1 == quadrant)
                {
                    App::GetInstance().meshUniformParams[i] = glm::vec4(0.f, 1.f, 0.f, 1.f);
                }
                else
                {
                    App::GetInstance().meshUniformParams[i] = glm::vec4(0.f, 0.f, 1.f, 1.f);
                }
            }
        }

        static int getQuadrant(float x, float y)
        {
            float radians = glm::atan(y, x);

            // ���Ƕ�ת��Ϊ[0, 2��]��Χ
            float degrees = glm::degrees(radians);

            // �жϽǶ����ڵ�����
            if (degrees > 0.f && degrees < 90.f) {
                return 1; // 0-90��
            }
            else if (degrees > 90.f && degrees < 180.f) {
                return 2; // 90-180��
            }
            else if (degrees > -180.f && degrees < -90.f) {
                return 3; // 180-270��
            }
            else if (degrees > -90.f && degrees < 0.f) {
                return 4; // 270-360��
            }
            return -1;
        }

        // mesh ����Դ �� ��Ⱦ�õľ������ݣ�һ���ɳ�������������
        //    mesh �� VAO������Ϊ�������������干��ͬһ�� mesh������������ mesh �����һ�� �� ��״��
        GLuint VAO = 0;
        size_t elementCount = 0;
        //    mvp ����
        //        �� �� ��תΪ �� �� �� �� 4����̬�� models ����
        std::vector<glm::mat4> models;
        glm::mat4 view;
        glm::mat4 projection;
        //    shader, ����Ϊ�������������干��ͬһ�� shader��
        OGLRenderer::Graphics::Shader s;
        //    ÿ���������� �� uniform ����
        std::vector<glm::vec4> meshUniformParams;

        GLFWwindow* window = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;
    };
}

#endif