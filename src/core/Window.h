#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include <vector>
#include <functional>
#include "Events.h"

// TODO absract the window to be able to support other Display Servers. Now
// this implementation only supports X11. Which will be fine for now.
class GWindow {
public:
    GWindow();
    ~GWindow();

    bool ready_for_render = true;
    int m_width;
    int m_height;
    int m_res_x;
    int m_res_y;

    bool initialize(int width, int height);
    bool poll_event(WindowEvent &event);

    Display* get_display();
    Window get_window();
    int get_screen();

    void toggle_fullscreen();
    void resize(int width, int height);
private:
    Display* p_display = nullptr;
    Window m_window;

    int m_screen;
    bool m_fullscreen;

    void set_fullscreen();
    void unset_fullscreen();
};
