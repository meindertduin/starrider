#pragma once

#include "../core/Window.h"
#include "Core.h"
#include "../core/Application.h"

class Renderer : EventObserver<WindowEvent> {
public:
    Renderer();
    ~Renderer();
    void set_color(const Color &color);
    void draw_line(const Point &p1, const Point &p2, const Color &color);
    void set_frame_pixel(int x_pos, int y_pos, uint32_t value);
    void clear_screen();
    bool render();

    void on_event(const WindowEvent &event) override;

    int m_height;
    int m_width;
private:
    GWindow *p_window;
    Visual* p_visual;
    XShmSegmentInfo m_shm_info;
    XImage* p_screen_image;
    GC m_gc;

    Application *p_app;

    uint32_t* m_framebuffer;

    bool setup_shared_memory();
    void shared_memory_resize();
    void create_framebuffer();
    void remove_shared_memory();
};
