#include "Window.h"
#include <stdio.h>
#include "Application.h"

#include "KeyMap.h"
#include "../graphics/XLib.h"

GWindow::GWindow() {

}

GWindow::~GWindow() {
    XLib::lib_quit();
}

bool GWindow::initialize(const WindowSettings &win_settings) {
    m_display_mode = WDisplayMode::Normal;
    XLib::lib_init(win_settings.width, win_settings.height, 0);
    create_screen_bitmap();

    return true;
}

bool GWindow::poll_event(WindowEvent &event) {
    bool has_event = XLib::poll_event(event);

    if (has_event && event.event_type == WindowEventType::WinExpose) {
        m_screen_bitmap.buffer = XLib::x_screen.buffer;
        m_screen_bitmap.w = XLib::x_screen.width;
        m_screen_bitmap.h = XLib::x_screen.height;
    }

    return has_event;
}

void GWindow::resize(int width, int height) {
    XLib::resize_window(width, height);

    WindowEvent e;
    e.body.expose_event.width = XLib::x_screen.width;
    e.body.expose_event.height = XLib::x_screen.height;
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

    if (XLib::set_win_float_mode() == 0)
        m_display_mode = WDisplayMode::Floating;
}

void GWindow::set_wind_normal_mode() {
    if (m_display_mode == WDisplayMode::Normal)
        return;

    if (XLib::set_win_normal_mode() == 0)
        m_display_mode = WDisplayMode::Normal;
}

void GWindow::set_fullscreen_mode() {
    if (m_display_mode == WDisplayMode::FullScreen)
        return;

    if (XLib::set_win_fullscreen_mode() != 0)
        m_display_mode = WDisplayMode::FullScreen;
}

int GWindow::get_width() {
    return XLib::x_screen.width;
}

int GWindow::get_height() {
    return XLib::x_screen.height;
}

inline void GWindow::create_screen_bitmap() {
    m_screen_bitmap = {
        .buffer = XLib::x_screen.buffer,
        .w = XLib::x_screen.width,
        .h = XLib::x_screen.height,
    };
}

ScreenBitmap* GWindow::get_screen_bitmap() {
    return &m_screen_bitmap;
}

void GWindow::render_screen() {
    XLib::render_screen();
}
