#ifndef __MOUSEEVENT_H__
#define __MOUSEEVENT_H__

namespace RyuRenderer::App::Events
{
    struct MouseEvent
    {
        enum EventType
        {
            MOUSE_NONE,
            MOUSE_MOVE,
            MOUSE_SCROLL,
            MOUSE_BUTTON,
            MOUSE_ENTER_OR_LEAVE
        };

        enum ButtonType
        {
            BUTTON_NONE,
            BUTTON_LEFT,
            BUTTON_RIGHT,
            BUTTON_MIDDLE,
            BUTTON_4,
            BUTTON_5,
            BUTTON_6,
            BUTTON_7,
            BUTTON_8
        };

        enum ButtonActionType
        {
            BUTTON_ACTION_NONE,
            BUTTON_ACTION_PRESS,
            BUTTON_ACTION_RELEASE
        };

        EventType Event = EventType::MOUSE_NONE;
        
        double MoveXPos = 0;
        double MoveYPos = 0;
        double ScrollXOffset = 0;
        double ScrollYOffset = 0;
        ButtonActionType ButtonAction = ButtonActionType::BUTTON_ACTION_NONE;
        ButtonType Button = ButtonType::BUTTON_NONE;
        bool IsEnteredWindow = false;
    };
}

#endif