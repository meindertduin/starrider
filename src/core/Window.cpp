#include "Window.h"
#include <stdio.h>
#include "Application.h"

#include "KeyMap.h"
#include "../graphics/XLib.h"

GWindow::GWindow() {

}

GWindow::~GWindow() {
    xlib_quit();
}

bool GWindow::initialize(const WindowSettings &win_settings) {
    m_display_mode = WDisplayMode::Normal;
    xlib_init(win_settings.width, win_settings.height);
    create_screen_bitmap();

    return true;
}

bool GWindow::poll_event(WindowEvent &event) {
    bool has_event = xpoll_event(event);

    if (has_event && event.event_type == WindowEventType::WinExpose) {
        m_screen_bitmap.buffer = x_screen.buffer;
        m_screen_bitmap.w = x_screen.w;
        m_screen_bitmap.h = x_screen.h;
    }

    return has_event;
}

void GWindow::resize(int width, int height) {
    xresize_window(width, height);

    WindowEvent e;
    e.body.expose_event.width = x_window.w;
    e.body.expose_event.height = x_window.h;
    e.event_type = WindowEventType::WinExpose;

    auto app = Application::get_instance();
    app->send_window_event(e);
}

void GWindow::set_win_display_mode(WDisplayMode mode) {
    switch(mode) {
        case WDisplayMode::Floating:
            set_win_float_mode();
            break;
        case WDisplayMode::FullScreen:
            set_fullscreen_mode();
            break;
        case WDisplayMode::Normal:
            set_wind_normal_mode();
            break;
    }
}

void GWindow::set_win_float_mode() {
    if (m_display_mode == WDisplayMode::Floating)
        return;

    if (xset_float_mode() == 0)
        m_display_mode = WDisplayMode::Floating;
}

void GWindow::set_wind_normal_mode() {
    if (m_display_mode == WDisplayMode::Normal)
        return;

    if (xset_normal_mode() == 0)
        m_display_mode = WDisplayMode::Normal;
}

void GWindow::set_fullscreen_mode() {
    if (m_display_mode == WDisplayMode::FullScreen)
        return;

    if (xset_fullscreen_mode() != 0)
        m_display_mode = WDisplayMode::FullScreen;
}

int GWindow::get_width() {
    return x_window.w;
}

int GWindow::get_height() {
    return x_window.h;
}

inline void GWindow::create_screen_bitmap() {
    m_screen_bitmap = {
        .buffer = x_screen.buffer,
        .w = x_screen.w,
        .h = x_screen.h,
    };
}

ScreenBitmap* GWindow::get_screen_bitmap() {
    return &m_screen_bitmap;
}

void GWindow::render_screen() {
    xrender_screen();
}
