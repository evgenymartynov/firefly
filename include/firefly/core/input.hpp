#ifndef FIREFLY_INPUT_HPP
#define FIREFLY_INPUT_HPP

////////////////////////////////////////////////////////////////////////

namespace ff {

    // holds user input information
    struct input
    {
        enum
        {
            ffPressed = 1,
            ffReleased = 0
        };

        struct text_event
        {
            int code;
            int state;
        };

        struct key_event
        {
            int  code;
            int  state;
            bool shift;
        };

        struct mousemove_event
        {
            int x, y;
        };

        struct mousebutton_event
        {
            int button;
            int x, y;
            int state;
        };

        struct mousewheel_event
        {
            int delta;
            int x, y;
        };

        enum
        {
            ffText = 1,
            ffKeyboard,
            ffMouseMove,
            ffMouseButton,
            ffMouseWheel
        };

        char source;

        union
        {
            text_event         text;
            key_event          key;
            mousemove_event    mouse_move;
            mousebutton_event  mouse_button;
            mousewheel_event   mouse_wheel;
        };
    };

    struct mouse_info
    {
        bool LMB, MMB, RMB;
        int  x, y;
    };

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
