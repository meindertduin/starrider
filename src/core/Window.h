#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include <vector>
#include <functional>
#include "Events.h"

enum class WDisplayMode {
    FullScreen,
    Floating,
    Normal
};

struct WindowSettings {
    int width;
    int height;
    WDisplayMode win_display_mode;
};

// TODO absract the window to be able to support other Display Servers. Now
// this implementation only supports X11. Which will be fine for now.
class GWindow {
public:
    GWindow();
    ~GWindow();

    bool ready_for_render = true;
    int m_width;
    int m_height;

    bool initialize(const WindowSettings &win_settings);
    bool poll_event(WindowEvent &event);
    void set_win_display_mode(WDisplayMode mode);

    Display* get_display();
    Window get_window();
    int get_screen();

    void resize(int width, int height);
private:
    Display* p_display = nullptr;
    Window m_window;
    Visual* p_visual = nullptr;

    int m_screen;
    bool m_fullscreen;
    WDisplayMode m_display_mode;

    void set_win_float_mode();
    void set_wind_normal_mode();
    void set_fullscreen_mode();
};
