#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>
#include <linux/input.h>

#include "ScreenTouch.h"
std::string exec(const char* cmd);
void write_event(int fd, __u16 type, __u16 code, __s32 value);
void write_touch_event(int fd, int x, int y);

ScreenTouch::ScreenTouch()
{
    //ctor
}

ScreenTouch::~ScreenTouch()
{
    XCloseDisplay(dpy);
}

void ScreenTouch::SetReverse(bool b)
{
    Reverse=b;
}

bool ScreenTouch::Init()
{
    int height=0,width=0;
    dpy = XOpenDisplay(NULL);
    if ( dpy == NULL) {
        perror("ERROR Opening XDisplay\n");
        return false;
    }
    Screen*  scrn = DefaultScreenOfDisplay(dpy);
    if(scrn->height > scrn->width){
        height = scrn->height;
        width = scrn->width;
    }
    else
    {
        width = scrn->height;
        height = scrn->width;
    }

    int halfheight = height/2;
    int halfwidth = width/2;
    std::string orientation = exec("lipc-get-prop com.lab126.winmgr orientation");
    if(orientation.size()<1) return false;
    orientation.pop_back(); //it retuns with a newline... took a while to figure why compares weren't working
    if(orientation.compare("U") == 0){
        //upright/portrait
        next_x = width;
        next_y = halfheight;
        prev_x = 0;
        prev_y = halfheight;
    }
    else{
        //landscape
        next_x = halfwidth;
        next_y = 0;
        prev_x = halfwidth;
        prev_y = height;
    }

    std::string devName="/dev/input/event";
    devName+= '0'+evdev;

    int eventfd= open( devName.c_str(), O_WRONLY);
    if(eventfd<0)
      {
        perror("ERROR opening evdev (touch screen) device\n");
        return false;
      }
    else{
        printf("opened event\n");
        eventfdTOUCH=eventfd;
        return true;
    }
}


void ScreenTouch::Act(int act)
{
    std::string new_brightness;
    int current_brightness;

    switch(act)
    {
        case key_next_page:
        if(Reverse){
            // mouse_click(dpy, 0, halfheight);
            write_touch_event(eventfdTOUCH, prev_x, prev_y);
        }
        else{
            // mouse_click(dpy, width, halfheight);
            write_touch_event(eventfdTOUCH, next_x, next_y);
        }
        break;

        case key_prev_page:
        if(Reverse){
            // mouse_click(dpy, width, halfheight);
            write_touch_event(eventfdTOUCH, next_x, next_y);
        }
        else{
            // mouse_click(dpy, 0, halfheight);
            write_touch_event(eventfdTOUCH, prev_x, prev_y);
        }
        break;

        case key_brightness_up:
        current_brightness = std::stoi( exec("lipc-get-prop com.lab126.powerd flIntensity"));
        if(current_brightness < 24){
            current_brightness++;
        }
        new_brightness = std::to_string(current_brightness);
        new_brightness = "lipc-set-prop com.lab126.powerd flIntensity " + new_brightness;
        system(new_brightness.c_str());
        break;

        case key_brightness_down:
        current_brightness = std::stoi(exec("lipc-get-prop com.lab126.powerd flIntensity"));
        if(current_brightness > 0){
            current_brightness--;
        }
        new_brightness = std::to_string(current_brightness);
        new_brightness = "lipc-set-prop com.lab126.powerd flIntensity " + new_brightness;
        system(new_brightness.c_str());
        break;

        case key_wake:
        std::string state = exec("lipc-get-prop com.lab126.powerd state");
        state.pop_back(); //remove newline from string
        if(state.compare("screenSaver") == 0){
            exec("lipc-set-prop com.lab126.powerd wakeUp 1");
        }
        break;
    }

    //keep awake. at least faking touch with xdotool, we needed this.
    //otherwise the kindle sleeps due to no touch - we probably need it here too.
    system("lipc-set-prop com.lab126.powerd -i touchScreenSaverTimeout 1");
}

void write_touch_event(int fd, int x, int y)
{
    //For kindle, get sequence from "evtest /dev/input/eventX"
    write_event(fd, EV_KEY, BTN_TOUCH, 1);
    write_event(fd, EV_ABS, ABS_MT_TRACKING_ID, 0);
    write_event(fd, EV_ABS, ABS_MT_POSITION_X , x);
    write_event(fd, EV_ABS, ABS_MT_POSITION_Y, y);
    write_event(fd, EV_SYN, 0, 0);
    write_event(fd, EV_KEY, BTN_TOOL_FINGER, 1);
    write_event(fd, EV_SYN, 0, 0);
    write_event(fd, EV_ABS, ABS_MT_TRACKING_ID , -1);
    write_event(fd, EV_SYN, 0, 0);
    write_event(fd, EV_KEY, BTN_TOUCH , 0);
    write_event(fd, EV_KEY, BTN_TOOL_FINGER, 0);
    write_event(fd, EV_SYN, 0, 0);
}

void write_event(int fd, __u16 type, __u16 code, __s32 value)
{
    int ret;
    struct input_event event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;
    ret = write(fd, &event, sizeof(event));
    if (ret < 0)
        perror("ERROR write_event");
    fsync(fd);
}


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}
