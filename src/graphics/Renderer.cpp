#include "Renderer.h"
#include <stdio.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>

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

    XMatchVisualInfo(p_window->get_display(), p_window->get_screen_num(), 32, DirectColor, &m_visual_info);
    p_visual = m_visual_info.visual;

    m_framebuffer = new int[m_width * m_height];
    for (auto i = 0u; i < m_width * m_height; ++i) {
		*(m_framebuffer+i) = 0xF0F0F06F;
	}
}

Renderer::~Renderer() {
    XFreeColormap(p_window->get_display(), m_colormap);

    delete[] m_framebuffer;
    p_visual = nullptr;
    p_window = nullptr;
}

void Renderer::set_color(const Color &color) {

}

void Renderer::render() {
    Visual *xvisual;
    Visual pixmap;

    auto image = XCreateImage(p_window->get_display(), xvisual, 24, ZPixmap, 0, (char*) m_framebuffer, m_width, m_height, 32, 0);
    XPutImage(p_window->get_display(), p_window->get_window(), m_gc, image, 0, 0, 0, 0, m_width, m_height);
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


