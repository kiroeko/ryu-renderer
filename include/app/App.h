#ifndef __APP_H__
#define __APP_H__

#include "GLFW/glfw3.h"

#include "app/AppSettings.h"
#include "app/render-pipeline/IRenderPipeline.h"
#include "common/Publisher.h"
#include "common/Singleton.h"

#include <iostream>

namespace RyuRenderer::App
{
    class App : public Common::Singleton<App>
    {
    public:
        ~App();

        bool Init(const AppSettings& settings);

        void Run(RyuRenderer::App::RenderPipeline::IRenderPipeline* p);

        int GetWindowWidth() const;

        int GetWindowHeight() const;

        Common::Publisher EventPublisher;
    private:
        void Clear();

        void setWindowIcon(const std::string& iconPath);

        static void onWindowSizeChanged(GLFWwindow* window, int width, int height);

        static void onWindowFocusChanged(GLFWwindow* window, int focused);

        static void onMouseMove(GLFWwindow* window, double xpos, double ypos);

        static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

        static void onMouseButton(GLFWwindow* window, int button, int action, int mods);

        static void onMouseEnter(GLFWwindow* window, int entered);

        static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

        GLFWwindow* window = nullptr;
        RyuRenderer::App::RenderPipeline::IRenderPipeline* renderPipeline = nullptr;
        int windowWidth = 0;
        int windowHeight = 0;
        double lastTickTimeInS = 0.0;
        bool IsFocused = true;
    };
}

#endif