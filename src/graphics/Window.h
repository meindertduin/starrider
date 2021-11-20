#pragma once

#include <X11/Xlib.h>

// TODO absract the window to be able to support other Display Servers. Now
// this implementation only supports X11. Which will be fine for now.
class GWindow {
public:
    GWindow();
    ~GWindow();
    bool initialize();
    Display* get_display();
    Window get_window();
    int get_screen_num();
private:
    Display* p_display;
    Window m_window;
    int m_screen;
};
