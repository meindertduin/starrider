#pragma once

#include "Window.h"

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
    Colormap m_colormap;
};
