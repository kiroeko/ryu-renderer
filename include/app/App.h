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

            if (settings.lockCursorToCenter)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            windowWidth = settings.WindowWidth;
            windowHeight = settings.WindowHeight;
            glfwSetFramebufferSizeCallback(window, onWindowSizeChanged);

            glfwSetCursorPosCallback(window, onMouseMove);
            glfwSetMouseButtonCallback(window, onMouseButton);
            glfwSetScrollCallback(window, onMouseScroll);
            glfwSetCursorEnterCallback(window, onMouseEnter);
            glfwSetKeyCallback(window, onKeyEvent);

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

        static void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
            std::cout << "Mouse position: (" << xpos << ", " << ypos << ")" << std::endl;
        }

        static void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
            const char* buttonName = "";
            switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:   buttonName = "LEFT";   break;
            case GLFW_MOUSE_BUTTON_RIGHT:  buttonName = "RIGHT";  break;
            case GLFW_MOUSE_BUTTON_MIDDLE: buttonName = "MIDDLE"; break;
            default:                      buttonName = "OTHER";   break;
            }

            const char* actionName = (action == GLFW_PRESS) ? "PRESSED" : "RELEASED";
            std::cout << "Mouse button: " << buttonName << " " << actionName << std::endl;
        }

        static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
            std::cout << "Mouse scroll: X=" << xoffset << ", Y=" << yoffset << std::endl;
        }

        static void onMouseEnter(GLFWwindow* window, int entered) {
            if (entered) {
                std::cout << "Mouse entered window" << std::endl;
            }
            else {
                std::cout << "Mouse left window" << std::endl;
            }
        }

        static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            std::string keyName;
            auto keyNameCStr = glfwGetKeyName(key, scancode);
            if (keyNameCStr)
            {
                keyName = keyNameCStr;
            }

            if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL) && (mods & GLFW_MOD_SHIFT)) {
                std::cout << "Save with extra options!" << std::endl;
            }
            // 检测字母键
            else if (!keyName.empty() && keyName[0] >= 'A' && keyName[0] <= 'Z') {
                if (mods & GLFW_MOD_SHIFT) {
                    std::cout << "Shift+" << keyName << std::endl;
                }
                else {
                    std::cout << "Key: " << keyName << std::endl;
                }
            }
        }

        GLFWwindow* window = nullptr;
        RyuRenderer::App::Pipeline::IPipeline* pipeline = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;
    };
}

#endif