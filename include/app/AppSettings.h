#ifndef __APPSETTINGS_H__
#define __APPSETTINGS_H__

#include <string>

namespace RyuRenderer::App
{
    struct AppSettings
    {
        std::string WindowName = "Ryu Renderer";
        std::string WindowIconPath = "res/icons/icon.png";
        int WindowWidth = 1920;
        int WindowHeight = 1080;
        int VSyncInterval = 1;
        bool HideCursor = true;
        bool LockCursorToCenter = true;
    };
}

#endif