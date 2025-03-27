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

            // 启用鼠标
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
            ClearEx();

            // 绑定 mesh 到 VAO
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

            // 设置4个物体的 model 矩阵
            // 第一个元素，原地不动
            glm::mat4 m1 = glm::identity<glm::mat4>();
            models.push_back(m1);

            // 第二个元素：沿 z 轴负方向（笛卡尔右手坐标系）绕原点转 90 度，也就是在 x-y 平面绕原点顺时针转 90 度。
            glm::mat4 m2 = glm::identity<glm::mat4>();
            m2 = glm::rotate(m2, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m2);

            // 第三个元素：沿 z 轴负方向（笛卡尔右手坐标系）绕原点转 180 度，也就是在 x-y 平面绕原点顺时针转 180 度。
            glm::mat4 m3 = glm::identity<glm::mat4>();
            m3 = glm::rotate(m3, glm::radians(180.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m3);

            // 第四个元素：沿 z 轴负方向（笛卡尔右手坐标系）绕原点转 270 度，也就是在 x-y 平面绕原点顺时针转 270 度。
            glm::mat4 m4 = glm::identity<glm::mat4>();
            m4 = glm::rotate(m4, glm::radians(270.0f), glm::vec3(0.0, 0.0, -1.0));
            models.push_back(m4);

            // 设置 view （代表摄像头的位置和方向的矩阵），这里把摄像头往 z 轴正方向移一个单位，看向 z 轴负方向，以确保 z 值为 0 的物体在自己前面。
            view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 1.0f), // 摄像机位置
                glm::vec3(0.0f, 0.0f, 0.0f), // 看向的目标点
                glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向，设为 Y 轴正方向
            );

            // 设置摄像机的投影矩阵，设为正交
            UpdateProjection();

            // 加载 shader
            s = OGLRenderer::Graphics::Shader("res/shaders/base.vs", "res/shaders/base.fs");
            s.Use();
            // 固定的矩阵参数
            s.SetUniformWithMatrixFloat("uView", view);

            // 设置物体的 shader 属性参数，这里是 RGBA 控制其颜色，默认给个蓝色
            for (int i = 0; i < 4; ++i)
            {
                meshUniformParams.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
            }
        }

        void UpdateProjection()
        {
            // 屏幕大小发生变化时，投影矩阵保持宽高比一致以防止变形
            float aspectRatio = 0.f;
            if (windowHeight != 0)
                aspectRatio = (float)windowWidth / (float)windowHeight;
            // 控制缩放的系数
            float orthoWidth = 100.0f;
            float orthoHeight = orthoWidth / aspectRatio;

            float nearp = 0.1f;
            float farp = 1000.0f;
            projection = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, nearp, farp);
        }

        // 这里我们简单合批渲染一下由不同材质参数的多个物件，
        //     实际上可以使用复杂的场景物体预剔除、LOD以及合批的策略，优化这一步的性能，但其实对于几个场景物体，也没啥优化空间。
        void renderScene()
        {
            s.SetUniformWithMatrixFloat("uProjection", projection);
            // 画 4 个屏幕物体
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

            // 将角度转换为[0, 2π]范围
            float degrees = glm::degrees(radians);

            // 判断角度所在的区间
            if (degrees > 0.f && degrees < 90.f) {
                return 1; // 0-90度
            }
            else if (degrees > 90.f && degrees < 180.f) {
                return 2; // 90-180度
            }
            else if (degrees > -180.f && degrees < -90.f) {
                return 3; // 180-270度
            }
            else if (degrees > -90.f && degrees < 0.f) {
                return 4; // 270-360度
            }
            return -1;
        }

        // mesh 数据源 和 渲染用的矩阵数据，一般由场景管理器管理
        //    mesh 的 VAO，这里为了性能所有物体共用同一个 mesh，这里描述的 mesh 大概是一个 ┌ 形状。
        GLuint VAO = 0;
        size_t elementCount = 0;
        //    mvp 矩阵
        //        把 ┌ 旋转为 ┌ ┐ ┘ └ 4个形态的 models 矩阵。
        std::vector<glm::mat4> models;
        glm::mat4 view;
        glm::mat4 projection;
        //    shader, 这里为了性能所有物体共用同一个 shader。
        OGLRenderer::Graphics::Shader s;
        //    每个场景物体 的 uniform 参数
        std::vector<glm::vec4> meshUniformParams;

        GLFWwindow* window = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;
    };
}

#endif