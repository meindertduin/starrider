#pragma once

#include "Window.h"
#include "Core.h"
#include <X11/Xutil.h>

class Renderer {
public:
    Renderer(GWindow* window);
    ~Renderer();
    void set_color(const Color &color);
    void draw_line(const Point &p1, const Point &p2, const Color &color);
    void draw_triangle(const Triangle &triangle);
    void clear_screen();
    bool render();
private:
    GWindow *p_window;
    Visual* p_visual;
    XShmSegmentInfo m_shm_info;
    XImage* p_screen_image;
    GC m_gc;

    int* m_framebuffer;
    int m_height = 800;
    int m_width = 800;

    bool setup_shared_memory();
    void draw_between_edges(const Edge &e1, const Edge &e2);
    void draw_span(const Span &span, const int &y);
};
