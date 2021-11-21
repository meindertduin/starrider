#pragma once

#include "Window.h"

#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

struct Color {
    u_int8_t r, g, b;
};

class Renderer {
public:
    Renderer(GWindow* window);
    ~Renderer();
    void set_color(const Color &color);
    void set_background_color(const Color &color);
    bool render();
private:
    GWindow *p_window;
    XShmSegmentInfo m_shm_info;
    XImage* p_screen_image;
    GC m_gc;
    XVisualInfo m_visual_info;
    Visual* p_visual;
    Colormap m_colormap;
    int* m_framebuffer;
    int m_height = 800;
    int m_width = 800;

    bool setup_shared_memory();
};
