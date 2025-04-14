#ifndef __WINDOWEVENT_H__
#define __WINDOWEVENT_H__

namespace RyuRenderer::App::Events
{
    struct WindowEvent
    {
        enum EventType
        {
            WINDOW_NONE,
            WINDOW_RESIZE,
            WINDOW_FOCUS
        };

        EventType Event = EventType::WINDOW_NONE;
        int Width = 0;
        int Height = 0;
        bool IsFocused = false;
    };
}

#endif