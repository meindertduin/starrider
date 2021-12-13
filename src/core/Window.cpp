#include "Window.h"
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include "Application.h"

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
    XChangeWindowAttributes(p_display, m_window, CWOverrideRedirect, &set_attr);
    XResizeWindow(p_display, m_window, m_width, m_height);

    XSelectInput(p_display, m_window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(p_display , m_window);

    return true;
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

bool GWindow::poll_event(XEvent &event) {
    if (XPending(p_display)) {
        XNextEvent(p_display, &event);
        if (event.type == Expose) {
            XWindowAttributes attributes;
            XGetWindowAttributes(p_display, m_window, &attributes);

            m_width = attributes.width;
            m_height = attributes.height;
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

    InputEvent e {
        .body = {
            .value = ((uint32_t)m_width << 16) | (uint32_t)m_height,
        },
        .event_type = EventType::Window,
    };

    app->send_window_event(e);
}
