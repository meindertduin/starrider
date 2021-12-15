#include "Window.h"
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include "Application.h"

#include <X11/Xutil.h>
#include "KeyMap.h"

GWindow::GWindow() {
}

GWindow::~GWindow() {
    XCloseDisplay(p_display);
}

bool GWindow::initialize(int width, int height) {
    m_width = width;
    m_height = height;

    p_display = XOpenDisplay(NULL);
    if (p_display == nullptr) {
        return false;
    }

    m_screen = DefaultScreen(p_display);

    int black_color = BlackPixel(p_display, m_screen);
    m_window = XCreateSimpleWindow(p_display, RootWindow(p_display, m_screen), 500, 100, m_width, m_height, 0, black_color, black_color);
    XSetWindowAttributes set_attr;
    set_attr.override_redirect = true;

    // TODO add some sort of setting for the window on startup
    //XChangeWindowAttributes(p_display, m_window, CWOverrideRedirect, &set_attr);
    XResizeWindow(p_display, m_window, m_width, m_height);

    XSelectInput(p_display, m_window, ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask);
    XMapWindow(p_display , m_window);

    return true;
}

void GWindow::toggle_fullscreen() {
    m_fullscreen = !m_fullscreen;

    Atom wmState = XInternAtom(p_display, "_NET_WM_STATE", False);
    Atom fullScreen = XInternAtom(p_display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = m_window;
    xev.xclient.message_type = wmState;
    xev.xclient.format= 32;
    xev.xclient.data.l[0] = (m_fullscreen ? 1 : 0);
    xev.xclient.data.l[1] = fullScreen;
    xev.xclient.data.l[2] = 0;

    XSendEvent(p_display, DefaultRootWindow(p_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

Display* GWindow::get_display() {
    return p_display;
}

Window GWindow::get_window() {
    return m_window;
}

int GWindow::get_screen_num() {
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
            case MotionNotify:
                event.body.mouse_event.x_pos = x_event.xmotion.x;
                event.body.mouse_event.y_pos = x_event.xmotion.y;
                event.event_type = WindowEventType::Mouse;
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
