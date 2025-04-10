#ifndef __APP_H__
#define __APP_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include "app/AppSettings.h"
#include "app/pipeline/IPipeline.h"
#include "common/Singleton.h"

#include <iostream>

namespace RyuRenderer::App
{
    class App : public RyuRenderer::Common::Singleton<App>
    {
    public:
        ~App()
        {
            Clear();
        }

        bool Init(const AppSettings& settings)
        {
            Clear();

            // init glfw
            glfwInit();

            // create window
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            window = glfwCreateWindow(settings.WindowWidth, settings.WindowHeight, settings.WindowName.c_str(), NULL, NULL);
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

            windowWidth = settings.WindowWidth;
            windowHeight = settings.WindowHeight;
            glfwSetFramebufferSizeCallback(window, onWindowSizeChanged);

            stbi_set_flip_vertically_on_load(true);

            return true;
        }

        void Run(RyuRenderer::App::Pipeline::IPipeline* p)
        {
            if (!window)
            {
                std::cerr << "App initialization incorrect, unable to run." << std::endl;
                return;
            }

            if (!p)
            {
                std::cerr << "App pipeline incorrect, unable to run." << std::endl;
                return;
            }
            pipeline = p;
            pipeline->init();

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
                pipeline->tick();

                // show render result
                glfwSwapBuffers(window);
            }
        }

        int GetWindowWidth() const
        {
            return windowWidth;
        }

        int GetWindowHeight() const
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
        RyuRenderer::App::Pipeline::IPipeline* pipeline = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;
    };
}

#endif