#ifndef SCREENTOUCH_H
#define SCREENTOUCH_H

#include <X11/extensions/XTest.h>

enum action_type {
    key_next_page,
    key_prev_page,
    key_brightness_up,
    key_brightness_down,
    key_wake
};

class ScreenTouch
{
    public:
        ScreenTouch();
        virtual ~ScreenTouch();

    protected:

    private:
        Display *dpy=nullptr;
        int evdev=1;
        int eventfdTOUCH=0;
        int next_x = 0,next_y = 0,
            prev_x = 0,prev_y = 0;
        bool Reverse=false;
    public:
        void SetReverse(bool b);
        bool Init();
        void Act(int act);
};

#endif // SCREENTOUCH_H
