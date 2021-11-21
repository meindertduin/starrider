#pragma once

#include "Window.h"

#include <X11/Xutil.h>

struct Color {
    u_int8_t r, g, b;
};

class Renderer {
public:
    Renderer(GWindow* window);
    ~Renderer();
    void set_color(const Color &color);
    void set_background_color(const Color &color);
    void render();
private:
    GWindow *p_window;
    GC m_gc;
    XVisualInfo m_visual_info;
    Visual* p_visual;
    Colormap m_colormap;
    int* m_framebuffer;
    int m_height = 800;
    int m_width = 800;
};
