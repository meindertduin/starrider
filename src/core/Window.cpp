#include "Window.h"
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include "Application.h"

#include <X11/Xutil.h>
#include "KeyMap.h"
#include "../graphics/XLib.h"

GWindow::GWindow() {

}

GWindow::~GWindow() {
    XCloseDisplay(p_display);
}

bool GWindow::initialize(const WindowSettings &win_settings) {
    m_width = win_settings.width;
    m_height = win_settings.height;
    m_display_mode = WDisplayMode::Normal;

    xlib_init(win_settings.width, win_settings.height);
    // p_display = XOpenDisplay(NULL);
    // if (p_display == nullptr) {
    //     return false;
    // }

    // m_screen = DefaultScreen(p_display);

    // int black_color = BlackPixel(p_display, m_screen);
    // m_window = XCreateSimpleWindow(p_display, RootWindow(p_display, m_screen), 500, 100, m_width, m_height, 0, black_color, black_color);

    // set_win_display_mode(win_settings.win_display_mode);

    // XSelectInput(p_display, m_window, ExposureMask | KeyPressMask | ButtonPressMask);
    // XMapWindow(p_display , m_window);

    return true;
}

Display* GWindow::get_display() {
    return p_display;
}

Window GWindow::get_window() {
    return m_window;
}

int GWindow::get_screen() {
    return m_screen;
}

bool GWindow::poll_event(WindowEvent &event) {
    if (XPending(p_display)) {
        XEvent x_event;
        XNextEvent(p_display, &x_event);

        switch(x_event.type) {
            case Expose:
                XWindowAttributes attributes;
                XGetWindowAttributes(p_display, m_window, &attributes);

                if (attributes.width == m_width && attributes.height == m_height) {
                    return false;
                }

                m_width = attributes.width;
                m_height = attributes.height;

                event.body.expose_event.width = m_width;
                event.body.expose_event.height = m_height;

                event.event_type = WindowEventType::WinExpose;

                break;
            case KeyPress:
                event.body.keyboard_event.keysym = XLookupKeysym(&x_event.xkey, 0);
                event.body.keyboard_event.mask = x_event.xkey.state;

                event.event_type = WindowEventType::KeyDown;
                break;
            default:
                break;
        }

        return true;
    }

    return false;
}

void GWindow::resize(int width, int height) {
    XResizeWindow(p_display, m_window, width, height);
    XMapWindow(p_display , m_window);

    m_width = width;
    m_height = height;

    auto app = Application::get_instance();

    WindowEvent e;

    e.body.expose_event.width = m_width;
    e.body.expose_event.height = m_height;
    e.event_type = WindowEventType::WinExpose;

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

    Atom window_type = XInternAtom(p_display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_dialog = XInternAtom(p_display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    auto status  =XChangeProperty(p_display, m_window, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_dialog, 1);
    XResizeWindow(p_display, m_window, m_width, m_height);

    if (status == 0)
        m_display_mode = WDisplayMode::Floating;
}

void GWindow::set_wind_normal_mode() {
    if (m_display_mode == WDisplayMode::Normal)
        return;

    Atom window_type = XInternAtom(p_display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_normal = XInternAtom(p_display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    auto status = XChangeProperty(p_display, m_window, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_normal, 1);

    if (status == 0)
        m_display_mode = WDisplayMode::Normal;
}

void GWindow::set_fullscreen_mode() {
    if (m_display_mode == WDisplayMode::FullScreen)
        return;

    Atom wm_state_atom = XInternAtom(p_display, "_NET_WM_STATE", False);
    Atom fullscreen_atom = XInternAtom(p_display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = m_window;
    xev.xclient.message_type = wm_state_atom;
    xev.xclient.format= 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen_atom;
    xev.xclient.data.l[2] = 0;

    auto status = XSendEvent(p_display, DefaultRootWindow(p_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    if (status != 0)
        m_display_mode = WDisplayMode::FullScreen;
}
