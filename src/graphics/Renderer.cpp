#include "Renderer.h"
#include <stdio.h>

Renderer::Renderer(GWindow* window) : p_window(window) {
    m_gc = XCreateGC(window->get_display(), window->get_window(), 0, nullptr);
    auto white_color = XWhitePixel(p_window->get_display(), p_window->get_screen_num());
    auto black_color = XBlackPixel(p_window->get_display(), p_window->get_screen_num());

    XSetBackground(p_window->get_display(), m_gc, black_color);
    XSetForeground(p_window->get_display(), m_gc, white_color);
    XSetFillStyle(window->get_display(), m_gc, FillSolid);
    XSetLineAttributes(window->get_display(), m_gc, 2, LineSolid, CapRound, JoinRound);

    Visual* default_visual = DefaultVisual(window->get_display(), DefaultScreen(window->get_display()));
    m_colormap = XCreateColormap(window->get_display(), window->get_window(), default_visual, AllocNone);
    XSync(window->get_display(), false);
}

Renderer::~Renderer() {
    XFreeColormap(p_window->get_display(), m_colormap);
}

void Renderer::set_color(const Color &color) {

}

void Renderer::render() {
    XFillRectangle(p_window->get_display(), p_window->get_window(), m_gc, 20, 20, 40, 40);
    XDrawLine(p_window->get_display(), p_window->get_window(), m_gc, 50, 50, 100, 100);
    XFlush(p_window->get_display());
}

void Renderer::set_background_color(const Color &color) {
    XColor x_color;
    x_color.red = color.r;
    x_color.green = color.g;
    x_color.blue = color.b;

    Status rc = XAllocColor(p_window->get_display(), m_colormap, &x_color);
    XSetBackground(p_window->get_display(), m_gc, x_color.pixel);

    if (rc == 0) {
        printf("Failed to allocate color\n");
    }

    XSync(p_window->get_display(), false);
}


