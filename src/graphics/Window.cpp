#include "Window.h"

GWindow::GWindow() {

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
    int white_color = WhitePixel(p_display, m_screen);

    m_window = XCreateSimpleWindow(p_display, RootWindow(p_display, m_screen), 10, 10, 200, 100, 0, black_color, black_color);

    XMapWindow(p_display , m_window);

    return true;
}
