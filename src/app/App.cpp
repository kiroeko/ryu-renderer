#include "app/App.h"

#include "glad/gl.h"
#include "stb/stb_image.h"

#include "app/events/KeyEvent.h"
#include "app/events/MouseEvent.h"
#include "app/events/WindowEvent.h"

namespace RyuRenderer::App
{
    App::~App()
    {
        Clear();
    }

    bool App::Init(const AppSettings& settings)
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
        windowWidth = settings.WindowWidth;
        windowHeight = settings.WindowHeight;

        SetWindowIcon(settings.WindowIconPath);

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
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapInterval(settings.VSyncInterval);

        // Window state
        glfwSetFramebufferSizeCallback(window, OnWindowSizeChanged);
        glfwSetWindowFocusCallback(window, OnWindowFocusChanged);

        // Inputs
        if (!settings.HideCursor && !settings.LockCursorToCenter)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Normal software
        else if (settings.HideCursor && !settings.LockCursorToCenter)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        else if (settings.HideCursor && settings.LockCursorToCenter)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS game
        else if (!settings.HideCursor && settings.LockCursorToCenter)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        glfwSetCursorPosCallback(window, OnMouseMove);
        glfwSetScrollCallback(window, OnMouseScroll);
        glfwSetMouseButtonCallback(window, OnMouseButton);
        glfwSetCursorEnterCallback(window, OnMouseEnter);
        glfwSetKeyCallback(window, OnKeyEvent);

        stbi_set_flip_vertically_on_load(true);

        return true;
    }

    void App::Run(RyuRenderer::App::RenderPipeline::IRenderPipeline* p)
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

        renderPipeline = p;
        renderPipeline->Init();

        // main loop
        while (!glfwWindowShouldClose(window))
        {
            // handle input events
            glfwPollEvents();

            // clear canvas
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            double currentTimeInS = glfwGetTime();
            double deltaTime = currentTimeInS - lastTickTimeInS;
            lastTickTimeInS = currentTimeInS;

            // render
            renderPipeline->Tick(deltaTime);

            // show render result
            glfwSwapBuffers(window);
        }
    }

    int App::GetWindowWidth() const
    {
        return windowWidth;
    }

    int App::GetWindowHeight() const
    {
        return windowHeight;
    }

    void App::Clear()
    {
        if (window)
            glfwTerminate();

        windowWidth = 0;
        windowHeight = 0;
        window = nullptr;
    }

    void App::SetWindowIcon(const std::string& iconPath)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load(iconPath.c_str(), &width, &height, &channels, 4);
        if (!pixels)
            return;

        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    }

    void App::OnWindowSizeChanged(GLFWwindow* window, int width, int height)
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

        Events::WindowEvent event;
        event.Event = Events::WindowEvent::EventType::WINDOW_RESIZE;
        event.Width = width;
        event.Height = height;
        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnWindowFocusChanged(GLFWwindow* window, int focused)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        App::GetInstance().IsFocused = focused;

        Events::WindowEvent event;
        event.Event = Events::WindowEvent::EventType::WINDOW_FOCUS;
        event.IsFocused = focused;
        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        Events::MouseEvent event;
        event.Event = Events::MouseEvent::EventType::MOUSE_MOVE;
        event.MoveXPos = xpos;
        event.MoveYPos = ypos;
        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        Events::MouseEvent event;
        event.Event = Events::MouseEvent::EventType::MOUSE_SCROLL;
        event.ScrollXOffset = xoffset;
        event.ScrollYOffset = yoffset;
        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        Events::MouseEvent event;
        event.Event = Events::MouseEvent::EventType::MOUSE_BUTTON;

        switch (action)
        {
        case GLFW_PRESS:
            event.ButtonAction = Events::MouseEvent::ButtonActionType::BUTTON_ACTION_PRESS;
            break;
        case GLFW_RELEASE:
            event.ButtonAction = Events::MouseEvent::ButtonActionType::BUTTON_ACTION_RELEASE;
            break;
        default:
            return;
        }

        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_RIGHT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_4:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_4;
            break;
        case GLFW_MOUSE_BUTTON_5:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_5;
            break;
        case GLFW_MOUSE_BUTTON_6:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_6;
            break;
        case GLFW_MOUSE_BUTTON_7:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_7;
            break;
        case GLFW_MOUSE_BUTTON_8:
            event.Button = Events::MouseEvent::ButtonType::BUTTON_8;
            break;
        default:
            return;
        }

        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnMouseEnter(GLFWwindow* window, int entered)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        Events::MouseEvent event;
        event.Event = Events::MouseEvent::EventType::MOUSE_ENTER_OR_LEAVE;
        event.IsEnteredWindow = entered;
        App::GetInstance().EventPublisher.Dispatch(event);
    }

    void App::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (!window)
            return;
        if (!App::GetInstance().window)
            return;
        if (window != App::GetInstance().window)
            return;

        auto keyNameCStr = glfwGetKeyName(key, scancode);
        if (!keyNameCStr)
            return;

        Events::KeyEvent event;

        switch (action)
        {
        case GLFW_PRESS:
            event.Action = Events::KeyEvent::ActionType::ACTION_PRESS;
            break;
        case GLFW_RELEASE:
            event.Action = Events::KeyEvent::ActionType::ACTION_RELEASE;
            break;
        default:
            return;
        }

        switch (mods)
        {
        case GLFW_MOD_SHIFT:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_SHIFT;
            break;
        case GLFW_MOD_CONTROL:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_CONTROL;
            break;
        case GLFW_MOD_ALT:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_ALT;
            break;
        case GLFW_MOD_SUPER:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_SUPER;
            break;
        case GLFW_MOD_CAPS_LOCK:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_CAPSLOCK;
            break;
        case GLFW_MOD_NUM_LOCK:
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NUMLOCK;
            break;
        }

        switch (key)
        {
        case GLFW_KEY_SPACE:
            event.Key = Events::KeyEvent::KeyType::KEY_SPACE;
            break;
        case GLFW_KEY_APOSTROPHE:
            event.Key = Events::KeyEvent::KeyType::KEY_APOSTROPHE;
            break;
        case GLFW_KEY_COMMA:
            event.Key = Events::KeyEvent::KeyType::KEY_COMMA;
            break;
        case GLFW_KEY_MINUS:
            event.Key = Events::KeyEvent::KeyType::KEY_MINUS;
            break;
        case GLFW_KEY_PERIOD:
            event.Key = Events::KeyEvent::KeyType::KEY_PERIOD;
            break;
        case GLFW_KEY_SLASH:
            event.Key = Events::KeyEvent::KeyType::KEY_SLASH;
            break;
        case GLFW_KEY_0:
            event.Key = Events::KeyEvent::KeyType::KEY_0;
            break;
        case GLFW_KEY_1:
            event.Key = Events::KeyEvent::KeyType::KEY_1;
            break;
        case GLFW_KEY_2:
            event.Key = Events::KeyEvent::KeyType::KEY_2;
            break;
        case GLFW_KEY_3:
            event.Key = Events::KeyEvent::KeyType::KEY_3;
            break;
        case GLFW_KEY_4:
            event.Key = Events::KeyEvent::KeyType::KEY_4;
            break;
        case GLFW_KEY_5:
            event.Key = Events::KeyEvent::KeyType::KEY_5;
            break;
        case GLFW_KEY_6:
            event.Key = Events::KeyEvent::KeyType::KEY_6;
            break;
        case GLFW_KEY_7:
            event.Key = Events::KeyEvent::KeyType::KEY_7;
            break;
        case GLFW_KEY_8:
            event.Key = Events::KeyEvent::KeyType::KEY_8;
            break;
        case GLFW_KEY_9:
            event.Key = Events::KeyEvent::KeyType::KEY_9;
            break;
        case GLFW_KEY_SEMICOLON:
            event.Key = Events::KeyEvent::KeyType::KEY_SEMICOLON;
            break;
        case GLFW_KEY_EQUAL:
            event.Key = Events::KeyEvent::KeyType::KEY_EQUAL;
            break;
        case GLFW_KEY_A:
            event.Key = Events::KeyEvent::KeyType::KEY_A;
            break;
        case GLFW_KEY_B:
            event.Key = Events::KeyEvent::KeyType::KEY_B;
            break;
        case GLFW_KEY_C:
            event.Key = Events::KeyEvent::KeyType::KEY_C;
            break;
        case GLFW_KEY_D:
            event.Key = Events::KeyEvent::KeyType::KEY_D;
            break;
        case GLFW_KEY_E:
            event.Key = Events::KeyEvent::KeyType::KEY_E;
            break;
        case GLFW_KEY_F:
            event.Key = Events::KeyEvent::KeyType::KEY_F;
            break;
        case GLFW_KEY_G:
            event.Key = Events::KeyEvent::KeyType::KEY_G;
            break;
        case GLFW_KEY_H:
            event.Key = Events::KeyEvent::KeyType::KEY_H;
            break;
        case GLFW_KEY_I:
            event.Key = Events::KeyEvent::KeyType::KEY_I;
            break;
        case GLFW_KEY_J:
            event.Key = Events::KeyEvent::KeyType::KEY_J;
            break;
        case GLFW_KEY_K:
            event.Key = Events::KeyEvent::KeyType::KEY_K;
            break;
        case GLFW_KEY_L:
            event.Key = Events::KeyEvent::KeyType::KEY_L;
            break;
        case GLFW_KEY_M:
            event.Key = Events::KeyEvent::KeyType::KEY_M;
            break;
        case GLFW_KEY_N:
            event.Key = Events::KeyEvent::KeyType::KEY_N;
            break;
        case GLFW_KEY_O:
            event.Key = Events::KeyEvent::KeyType::KEY_O;
            break;
        case GLFW_KEY_P:
            event.Key = Events::KeyEvent::KeyType::KEY_P;
            break;
        case GLFW_KEY_Q:
            event.Key = Events::KeyEvent::KeyType::KEY_Q;
            break;
        case GLFW_KEY_R:
            event.Key = Events::KeyEvent::KeyType::KEY_R;
            break;
        case GLFW_KEY_S:
            event.Key = Events::KeyEvent::KeyType::KEY_S;
            break;
        case GLFW_KEY_T:
            event.Key = Events::KeyEvent::KeyType::KEY_T;
            break;
        case GLFW_KEY_U:
            event.Key = Events::KeyEvent::KeyType::KEY_U;
            break;
        case GLFW_KEY_V:
            event.Key = Events::KeyEvent::KeyType::KEY_V;
            break;
        case GLFW_KEY_W:
            event.Key = Events::KeyEvent::KeyType::KEY_W;
            break;
        case GLFW_KEY_X:
            event.Key = Events::KeyEvent::KeyType::KEY_X;
            break;
        case GLFW_KEY_Y:
            event.Key = Events::KeyEvent::KeyType::KEY_Y;
            break;
        case GLFW_KEY_Z:
            event.Key = Events::KeyEvent::KeyType::KEY_Z;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT_BRACKET;
            break;
        case GLFW_KEY_BACKSLASH:
            event.Key = Events::KeyEvent::KeyType::KEY_BACKSLASH;
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT_BRACKET;
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            event.Key = Events::KeyEvent::KeyType::KEY_GRAVE_ACCENT;
            break;
        case GLFW_KEY_WORLD_1:
            event.Key = Events::KeyEvent::KeyType::KEY_WORLD_1;
            break;
        case GLFW_KEY_WORLD_2:
            event.Key = Events::KeyEvent::KeyType::KEY_WORLD_2;
            break;
        case GLFW_KEY_ESCAPE:
            event.Key = Events::KeyEvent::KeyType::KEY_ESCAPE;
            break;
        case GLFW_KEY_ENTER:
            event.Key = Events::KeyEvent::KeyType::KEY_ENTER;
            break;
        case GLFW_KEY_TAB:
            event.Key = Events::KeyEvent::KeyType::KEY_TAB;
            break;
        case GLFW_KEY_BACKSPACE:
            event.Key = Events::KeyEvent::KeyType::KEY_BACKSPACE;
            break;
        case GLFW_KEY_INSERT:
            event.Key = Events::KeyEvent::KeyType::KEY_INSERT;
            break;
        case GLFW_KEY_DELETE:
            event.Key = Events::KeyEvent::KeyType::KEY_DELETE;
            break;
        case GLFW_KEY_RIGHT:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT;
            break;
        case GLFW_KEY_LEFT:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT;
            break;
        case GLFW_KEY_DOWN:
            event.Key = Events::KeyEvent::KeyType::KEY_DOWN;
            break;
        case GLFW_KEY_UP:
            event.Key = Events::KeyEvent::KeyType::KEY_UP;
            break;
        case GLFW_KEY_PAGE_UP:
            event.Key = Events::KeyEvent::KeyType::KEY_PAGE_UP;
            break;
        case GLFW_KEY_PAGE_DOWN:
            event.Key = Events::KeyEvent::KeyType::KEY_PAGE_DOWN;
            break;
        case GLFW_KEY_HOME:
            event.Key = Events::KeyEvent::KeyType::KEY_HOME;
            break;
        case GLFW_KEY_END:
            event.Key = Events::KeyEvent::KeyType::KEY_END;
            break;
        case GLFW_KEY_CAPS_LOCK:
            event.Key = Events::KeyEvent::KeyType::KEY_CAPS_LOCK;
            break;
        case GLFW_KEY_SCROLL_LOCK:
            event.Key = Events::KeyEvent::KeyType::KEY_SCROLL_LOCK;
            break;
        case GLFW_KEY_NUM_LOCK:
            event.Key = Events::KeyEvent::KeyType::KEY_NUM_LOCK;
            break;
        case GLFW_KEY_PRINT_SCREEN:
            event.Key = Events::KeyEvent::KeyType::KEY_PRINT_SCREEN;
            break;
        case GLFW_KEY_PAUSE:
            event.Key = Events::KeyEvent::KeyType::KEY_PAUSE;
            break;
        case GLFW_KEY_F1:
            event.Key = Events::KeyEvent::KeyType::KEY_F1;
            break;
        case GLFW_KEY_F2:
            event.Key = Events::KeyEvent::KeyType::KEY_F2;
            break;
        case GLFW_KEY_F3:
            event.Key = Events::KeyEvent::KeyType::KEY_F3;
            break;
        case GLFW_KEY_F4:
            event.Key = Events::KeyEvent::KeyType::KEY_F4;
            break;
        case GLFW_KEY_F5:
            event.Key = Events::KeyEvent::KeyType::KEY_F5;
            break;
        case GLFW_KEY_F6:
            event.Key = Events::KeyEvent::KeyType::KEY_F6;
            break;
        case GLFW_KEY_F7:
            event.Key = Events::KeyEvent::KeyType::KEY_F7;
            break;
        case GLFW_KEY_F8:
            event.Key = Events::KeyEvent::KeyType::KEY_F8;
            break;
        case GLFW_KEY_F9:
            event.Key = Events::KeyEvent::KeyType::KEY_F9;
            break;
        case GLFW_KEY_F10:
            event.Key = Events::KeyEvent::KeyType::KEY_F10;
            break;
        case GLFW_KEY_F11:
            event.Key = Events::KeyEvent::KeyType::KEY_F11;
            break;
        case GLFW_KEY_F12:
            event.Key = Events::KeyEvent::KeyType::KEY_F12;
            break;
        case GLFW_KEY_F13:
            event.Key = Events::KeyEvent::KeyType::KEY_F13;
            break;
        case GLFW_KEY_F14:
            event.Key = Events::KeyEvent::KeyType::KEY_F14;
            break;
        case GLFW_KEY_F15:
            event.Key = Events::KeyEvent::KeyType::KEY_F15;
            break;
        case GLFW_KEY_F16:
            event.Key = Events::KeyEvent::KeyType::KEY_F16;
            break;
        case GLFW_KEY_F17:
            event.Key = Events::KeyEvent::KeyType::KEY_F17;
            break;
        case GLFW_KEY_F18:
            event.Key = Events::KeyEvent::KeyType::KEY_F18;
            break;
        case GLFW_KEY_F19:
            event.Key = Events::KeyEvent::KeyType::KEY_F19;
            break;
        case GLFW_KEY_F20:
            event.Key = Events::KeyEvent::KeyType::KEY_F20;
            break;
        case GLFW_KEY_F21:
            event.Key = Events::KeyEvent::KeyType::KEY_F21;
            break;
        case GLFW_KEY_F22:
            event.Key = Events::KeyEvent::KeyType::KEY_F22;
            break;
        case GLFW_KEY_F23:
            event.Key = Events::KeyEvent::KeyType::KEY_F23;
            break;
        case GLFW_KEY_F24:
            event.Key = Events::KeyEvent::KeyType::KEY_F24;
            break;
        case GLFW_KEY_F25:
            event.Key = Events::KeyEvent::KeyType::KEY_F25;
            break;
        case GLFW_KEY_KP_0:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_0;
            break;
        case GLFW_KEY_KP_1:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_1;
            break;
        case GLFW_KEY_KP_2:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_2;
            break;
        case GLFW_KEY_KP_3:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_3;
            break;
        case GLFW_KEY_KP_4:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_4;
            break;
        case GLFW_KEY_KP_5:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_5;
            break;
        case GLFW_KEY_KP_6:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_6;
            break;
        case GLFW_KEY_KP_7:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_7;
            break;
        case GLFW_KEY_KP_8:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_8;
            break;
        case GLFW_KEY_KP_9:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_9;
            break;
        case GLFW_KEY_KP_DECIMAL:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_DECIMAL;
            break;
        case GLFW_KEY_KP_DIVIDE:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_DIVIDE;
            break;
        case GLFW_KEY_KP_MULTIPLY:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_MULTIPLY;
            break;
        case GLFW_KEY_KP_SUBTRACT:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_SUBTRACT;
            break;
        case GLFW_KEY_KP_ADD:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_ADD;
            break;
        case GLFW_KEY_KP_ENTER:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_ENTER;
            break;
        case GLFW_KEY_KP_EQUAL:
            event.Key = Events::KeyEvent::KeyType::KEY_KP_EQUAL;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT_SHIFT;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT_CONTROL;
            break;
        case GLFW_KEY_LEFT_ALT:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT_ALT;
            break;
        case GLFW_KEY_LEFT_SUPER:
            event.Key = Events::KeyEvent::KeyType::KEY_LEFT_SUPER;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT_SHIFT;
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT_CONTROL;
            break;
        case GLFW_KEY_RIGHT_ALT:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT_ALT;
            break;
        case GLFW_KEY_RIGHT_SUPER:
            event.Key = Events::KeyEvent::KeyType::KEY_RIGHT_SUPER;
            break;
        case GLFW_KEY_MENU:
            event.Key = Events::KeyEvent::KeyType::KEY_MENU;
            break;
        default:
            return;
        }

        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_SHIFT &&
            (event.Key == Events::KeyEvent::KeyType::KEY_LEFT_SHIFT || event.Key == Events::KeyEvent::KeyType::KEY_RIGHT_SHIFT))
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }
        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_CONTROL &&
            (event.Key == Events::KeyEvent::KeyType::KEY_LEFT_CONTROL || event.Key == Events::KeyEvent::KeyType::KEY_RIGHT_CONTROL))
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }
        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_ALT &&
            (event.Key == Events::KeyEvent::KeyType::KEY_LEFT_ALT || event.Key == Events::KeyEvent::KeyType::KEY_RIGHT_ALT))
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }
        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_SUPER &&
            (event.Key == Events::KeyEvent::KeyType::KEY_LEFT_SUPER || event.Key == Events::KeyEvent::KeyType::KEY_RIGHT_SUPER))
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }
        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_CAPSLOCK &&
            event.Key == Events::KeyEvent::KeyType::KEY_CAPS_LOCK)
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }
        if (event.ModifierKey == Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NUMLOCK &&
            event.Key == Events::KeyEvent::KeyType::KEY_NUM_LOCK)
        {
            event.ModifierKey = Events::KeyEvent::ModifierKeyType::MODIFIER_KEY_NONE;
        }

        event.Name = keyNameCStr;
        App::GetInstance().EventPublisher.Dispatch(event);
    }
}