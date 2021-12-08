#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include <vector>
#include <functional>

// TODO absract the window to be able to support other Display Servers. Now
// this implementation only supports X11. Which will be fine for now.
class GWindow {
public:
    GWindow(int width, int height);
    ~GWindow();

    bool ready_for_render = true;

    bool initialize();
    bool poll_event(XEvent &event);

    Display* get_display();
    Window get_window();
    int get_screen_num();
    int m_width;
    int m_height;

    void resize(int width, int height);
    void set_on_resize(std::function<void(void)> on_resize);
private:
    Display* p_display;
    Window m_window;
    int m_screen;

    std::vector<std::function<void(void)>> m_resize_callbacks;
};
