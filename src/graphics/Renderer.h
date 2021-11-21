#pragma once

#include "Window.h"

#include <X11/Xutil.h>
#include <X11/extensions/dbe.h>

struct Color {
    u_int8_t r, g, b;
};

class Renderer {
public:
    Renderer(GWindow* window);
    ~Renderer();
    void set_color(const Color &color);
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
};
