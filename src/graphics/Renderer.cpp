#include <stdio.h>
#include <math.h>

#include "../core/Window.h"
#include "Renderer.h"

Renderer::Renderer() {
    p_app = Application::get_instance();
    p_window = p_app->get_window();

    m_width = p_window->get_width();
    m_height = p_window->get_height();

    create_framebuffer();

    p_screen_bitmap = p_window->get_screen_bitmap();
    p_app->listen(this, WindowEventType::WinExpose);
}

Renderer::~Renderer() {
    p_app->unlisten(this, WindowEventType::WinExpose);

    delete[] p_framebuffer;
}

void Renderer::set_color(const Color &color) {

}

void Renderer::on_event(const WindowEvent &event) {
    if (event.event_type == WindowEventType::WinExpose) {
        m_width = event.body.expose_event.width;
        m_height = event.body.expose_event.height;

        delete[] p_framebuffer;

        create_framebuffer();
    }
}

void Renderer::create_framebuffer() {
    p_framebuffer = new Pixel[m_width * m_height];
    std::fill(p_framebuffer, p_framebuffer + m_width * m_height, Pixel {.value = 0x00000000});
}

bool Renderer::render_framebuffer() {
    while (!p_window->ready_for_render) {
        // block untill drawing is complete
    }

    for (int y_out = 0; y_out < m_height; y_out++) {
        for (int x_out = 0; x_out < m_width; x_out++) {
            auto value = p_framebuffer[m_width * y_out + x_out];
		    *(p_screen_bitmap->buffer + m_width * y_out + x_out) = value.value;
        }
    }

    p_window->render_screen();

    return true;
}

void Renderer::draw_line(const Point &p1, const Point &p2, const Color &color) {
    if (p1.x > m_width || p1.y > m_height || p2.x > m_width || p2.y > m_height) {
        return;
    }

    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    u_int32_t p_code = color.to_uint32();

    if (dx == 0 && dy == 0) {
        p_framebuffer[m_width * p1.y + p1.x].value = p_code;
    }

    if (dx > dy) {
        int xmin, xmax;

        if (p1.x < p2.x) {
            xmin = p1.x;
            xmax = p2.x;
        } else {
            xmin = p2.x;
            xmax = p1.x;
        }

        float slope = (float) dy / (float) dx;
        for (auto x = xmin; x <= xmax; x++) {
            int y = std::floor(p1.y + ((x - p1.x) * slope));
            (*(p_framebuffer + ((m_width * y) + x))).value = p_code;
        }
    } else {
        int ymin, ymax;

        if (p1.y < p2.y) {
            ymin = p1.y;
            ymax = p2.y;
        } else {
            ymin = p2.y;
            ymax = p1.y;
        }

        float slope = (float) dx / (float) dy;
        for (auto y = ymin; y <= ymax; y++) {
            int x = std::floor(p1.x + ((y - p1.y) * slope));
            (*(p_framebuffer + ((m_width * y) + x))).value = p_code;
        }
    }
}

void Renderer::clear_screen() {
    Pixel value = {
        .value = 0x00000000
    };

    std::fill(p_framebuffer, p_framebuffer + m_width * m_height, value);
}

void Renderer::set_frame_pixel(int x_pos, int y_pos, uint32_t value) {
    p_framebuffer[m_width * y_pos + x_pos].value = value;
}

void Renderer::set_frame_pixel(int x_pos, int y_pos, const Pixel &value) {
    p_framebuffer[m_width * y_pos + x_pos] = value;
}

inline Pixel Renderer::get_pixel(int x_pos, int y_pos) {
    return p_framebuffer[m_width * y_pos + x_pos];
}


void Renderer::render_texture(const Texture &texture, const Rect &src, const Rect &dest) {
    if(dest.x_pos >= 0 && dest.width + dest.x_pos <= m_width
            && dest.y_pos >= 0 && dest.height + dest.y_pos <= m_height)
    {
        float x_step = (float)src.width / (float)dest.width;
        float y_step = (float)src.height / (float)dest.height;

        float y = 0;
        uint32_t a, r, g, b;

        for (int y_out = 0; y_out < dest.height; y_out++) {
            float x = 0;
            for (int x_out = 0; x_out < dest.width; x_out++) {
                auto pixel = texture.get_pixel(x + src.x_pos, y + src.y_pos);
                pixel.rgba565_from_16bit(r, g, b, a);

                if (a > 0) {
                    p_framebuffer[m_width * (y_out + dest.y_pos) + (x_out + dest.x_pos)].value =
                        rgba_bit((r << 3), (g << 2), (b << 3), a);
                }

                x += x_step;
            }
            y += y_step;
        }
    }
}

void Renderer::render_text(std::string text, const TTFFont &font, const Point &point) {
    Rect src;
    src.x_pos = 0;
    src.y_pos = 0;

    int dest_x_pos = point.x;

    for (auto c : text) {
        auto glyph = font.get_glyph(c);
        src.width = glyph.width;
        src.height = glyph.height;

        // fonts are rendered from top to bottom, so we need to offset smaller glyphs
        int render_from = font.get_font_size() - src.height;
        Rect dest = src;
        dest.x_pos = dest_x_pos;
        dest.y_pos = point.y + render_from;
        dest.width = glyph.width;
        dest.height = glyph.height;

        render_texture(glyph.texture, src, dest);
        dest_x_pos += glyph.advance;
    }
}

