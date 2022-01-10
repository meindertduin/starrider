#pragma once

#include <vector>
#include <functional>
#include "Events.h"

#include <memory>

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

struct ScreenBitmap {
    uint32_t* buffer;
    int w, h;
};

// TODO absract the window to be able to support other Display Servers. Now
// this implementation only supports X11. Which will be fine for now.
class GWindow {
public:
    GWindow();
    ~GWindow();

    bool ready_for_render = true;

    int get_width();
    int get_height();
    ScreenBitmap* get_screen_bitmap();

    bool initialize(const WindowSettings &win_settings);
    bool poll_event(WindowEvent &event);
    void set_win_display_mode(WDisplayMode mode);
    void render_screen();

    void resize(int width, int height);
private:
    ScreenBitmap m_screen_bitmap;

    bool m_fullscreen;
    WDisplayMode m_display_mode;

    void set_win_float_mode();
    void set_wind_normal_mode();
    void set_fullscreen_mode();
    inline void create_screen_bitmap();
};
