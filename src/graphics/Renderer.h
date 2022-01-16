#pragma once

#include "../core/Window.h"
#include "Core.h"
#include "../core/Application.h"
#include "Texture.h"
#include "Font.h"

#include <memory>

using Math::Point2D;

class Renderer : EventObserver<WindowEvent> {
public:
    Renderer();
    ~Renderer();

    void set_color(const Color &color);
    void draw_line(const Point &p1, const Point &p2, const Color &color);
    void clear_screen();
    bool render_framebuffer();
    void render_texture(const Texture &texture, const Rect &src, const Rect &dest);
    void render_text(std::string text, const TTFFont &font, const Point &point);
    void on_event(const WindowEvent &event) override;

    void set_frame_pixel(int x_pos, int y_pos, uint32_t value);
    void set_frame_pixel(int x_pos, int y_pos, const Pixel &value);

    Pixel* get_framebuffer() {
        return p_framebuffer;
    }

    int m_height;
    int m_width;
private:
    GWindow *p_window = nullptr;

    ScreenBitmap *p_screen_bitmap;

    std::shared_ptr<Application> p_app {nullptr};
    Pixel* p_framebuffer = nullptr;

    void create_framebuffer();
    Pixel get_pixel(int x_pos, int y_pos);
};
