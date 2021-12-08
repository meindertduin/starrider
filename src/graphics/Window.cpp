#include "Window.h"
#include <X11/extensions/XShm.h>
#include <stdio.h>

GWindow::GWindow(int width, int height) : m_width(width), m_height(height) {

}

GWindow::~GWindow() {
    XCloseDisplay(p_display);
}

bool GWindow::initialize() {
    p_display = XOpenDisplay(NULL);
    if (p_display == nullptr) {
        return false;
    }
    m_screen = DefaultScreen(p_display);

    int black_color = BlackPixel(p_display, m_screen);
    m_window = XCreateSimpleWindow(p_display, RootWindow(p_display, m_screen), 500, 100, m_width, m_height, 0, black_color, black_color);
    XSetWindowAttributes set_attr;
    set_attr.override_redirect = true;
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
        return true;
    }

    return false;
}

void GWindow::resize(int width, int height) {
    XResizeWindow(p_display, m_window, width, height);
    XMapWindow(p_display , m_window);

    m_width = width;
    m_height = height;

    for (auto callback : m_resize_callbacks) {
        callback();
    }
}

void GWindow::set_on_resize(std::function<void(void)> on_resize) {
    m_resize_callbacks.push_back(on_resize);
}
