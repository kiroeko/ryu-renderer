#ifndef __APPSETTINGS_H__
#define __APPSETTINGS_H__

#include <string>

namespace RyuRenderer::App
{
    struct AppSettings
    {
        std::string WindowName;
        int WindowWidth = 1920;
        int WindowHeight = 1080;
    };
}

#endif