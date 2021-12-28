#pragma once

#include "../core/Window.h"
#include "Core.h"
#include "../core/Application.h"
#include "Texture.h"

class Renderer : EventObserver<WindowEvent> {
public:
    Renderer();
    ~Renderer();
    void set_color(const Color &color);
    void draw_line(const Point &p1, const Point &p2, const Color &color);
    void set_frame_pixel(int x_pos, int y_pos, uint32_t value);
    void set_frame_pixel(int x_pos, int y_pos, const Pixel &value);
    void clear_screen();
    bool render();

    void render_texture(const Texture &texture, const Rect &src, const Rect &dest);
    void on_event(const WindowEvent &event) override;

    int m_height;
    int m_width;
private:
    GWindow *p_window = nullptr;
    Visual* p_visual = nullptr;
    XImage* p_screen_image = nullptr;

    XShmSegmentInfo m_shm_info;
    GC m_gc;

    Application *p_app = nullptr;
    Pixel* p_framebuffer = nullptr;

    bool setup_shared_memory();
    void shared_memory_resize();
    void create_framebuffer();
    void remove_shared_memory();
    Pixel get_pixel(int x_pos, int y_pos);
};
