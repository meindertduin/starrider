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
    void set_frame_pixel(int x_pos, int y_pos, uint32_t value);
    void clear_screen();
    bool render();

    int m_height = 800;
    int m_width = 800;
private:
    GWindow *p_window;
    Visual* p_visual;
    XShmSegmentInfo m_shm_info;
    XImage* p_screen_image;
    GC m_gc;

    uint32_t* m_framebuffer;

    bool setup_shared_memory();
};
