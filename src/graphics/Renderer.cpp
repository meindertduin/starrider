#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <X11/Xutil.h>

#include "../core/Window.h"
#include "Renderer.h"


Renderer::Renderer() {
    p_app = Application::get_instance();
    p_window = p_app->get_window();

    m_width = p_window->m_width;
    m_height = p_window->m_height;

    p_app->listen(this, WindowEventType::WinExpose);

    m_gc = XCreateGC(p_window->get_display(), p_window->get_window(), 0, nullptr);
    auto black_color = XBlackPixel(p_window->get_display(), p_window->get_screen());

    XSetBackground(p_window->get_display(), m_gc, black_color);

    p_visual = DefaultVisual(p_window->get_display(), DefaultScreen(p_window->get_display()));

    create_framebuffer();
    setup_shared_memory();

    XSync(p_window->get_display(), false);
}

Renderer::~Renderer() {
    p_app->unlisten(this, WindowEventType::WinExpose);

    XShmDetach(p_window->get_display(), &m_shm_info);
    shmdt(m_shm_info.shmaddr);
    shmctl(m_shm_info.shmid, IPC_RMID, 0);

    XDestroyImage(p_screen_image);

    if (p_framebuffer != nullptr)
       delete[] p_framebuffer;

    p_visual = nullptr;
    p_window = nullptr;
}

void Renderer::set_color(const Color &color) {

}

void Renderer::on_event(const WindowEvent &event) {
    if (event.event_type == WindowEventType::WinExpose) {
        m_width = event.body.expose_event.width;
        m_height = event.body.expose_event.height;

        if (p_framebuffer != nullptr)
            delete[] p_framebuffer;

        create_framebuffer();
        shared_memory_resize();
    }
}

void Renderer::create_framebuffer() {
    p_framebuffer = new Pixel[m_width * m_height];
    for (auto i = 0u; i < m_width * m_height; ++i) {
        (*(p_framebuffer+i)).value = 0x00000000;
    }
}

bool Renderer::render() {
    while (!p_window->ready_for_render) {
        // block untill drawing is complete
    }

    // copy all the data from the frame_buffer to the shm buffer
    for (auto i = 0u; i < m_width *m_height; i++) {
		*((int*) m_shm_info.shmaddr+i) = (*(p_framebuffer + i)).value;
	}

    Status status = XShmPutImage(p_window->get_display(), p_window->get_window(), m_gc, p_screen_image, 0, 0, 0, 0, m_width, m_height, true);

    if (status == 0) {
        printf("Something went wrong with rendering the shm Image\n");
        return false;
    }


    return true;
}



bool Renderer::setup_shared_memory() {
    auto shm_available = XShmQueryExtension(p_window->get_display());
    if (shm_available == 0) {
        printf("Shared memory not available\n");
        return false;
    }

    p_screen_image = XShmCreateImage(p_window->get_display(), p_visual, 24, ZPixmap, nullptr, &m_shm_info, m_width, m_height);

    m_shm_info.shmid = shmget(IPC_PRIVATE, p_screen_image->bytes_per_line * p_screen_image->height, IPC_CREAT | 0777);
    if (m_shm_info.shmid == -1) {
        printf("Failed to get shmid\n");
        return false;
    }

    m_shm_info.shmaddr = p_screen_image->data = (char*) shmat(m_shm_info.shmid, nullptr, 0);
    m_shm_info.readOnly = false;

    auto shm_attach = XShmAttach(p_window->get_display(), &m_shm_info);
    if (shm_attach == 0) {
        printf("Failed to attach shm_info\n");
        return false;
    }

    return true;
}

void Renderer::shared_memory_resize() {
    // TODO there might be a better way to implement resizing the shared memory.
    // Now I wanted something to work, but to improve checkout: https://stackoverflow.com/questions/30630051/how-do-i-implement-dynamic-shared-memory-resizing
    remove_shared_memory();
    setup_shared_memory();
}

void Renderer::remove_shared_memory() {
    XShmDetach(p_window->get_display(), &m_shm_info);
    shmdt(m_shm_info.shmaddr);
    shmctl(m_shm_info.shmid, IPC_RMID, 0);
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
    for (auto i = 0u; i < m_width * m_height; ++i) {
		(*(p_framebuffer+i)).value = 0x00000000;
	}
}

void Renderer::set_frame_pixel(int x_pos, int y_pos, uint32_t value) {
    if (x_pos < m_width) {
        p_framebuffer[m_width * y_pos + x_pos].value = value;
    }
}

void Renderer::set_frame_pixel(int x_pos, int y_pos, const Pixel &value) {
    p_framebuffer[m_width * y_pos + x_pos] = value;
}

Pixel Renderer::get_pixel(int x_pos, int y_pos) {
    return p_framebuffer[m_width * y_pos + x_pos];
}


void Renderer::render_texture(const Texture &texture, const Rect &src, const Rect &dest) {
    if (dest.x_pos >= 0 && dest.width + dest.x_pos <= m_width
            && dest.y_pos >= 0 && dest.height + dest.y_pos <= m_height)
    {
        float x_step = (float)src.width / (float)dest.width;
        float y_step = (float)src.height / (float)dest.height;

        float y = 0;
        for (int y_out = 0; y_out < dest.height; y_out++) {
            float x = 0;
            for (int x_out = 0; x_out < dest.width; x_out++) {
                auto pixel = texture.get_pixel(std::round(x + src.x_pos), std::round(y + src.y_pos));

                if (pixel.rgba.alpha > 0) {
                    if (pixel.rgba.alpha != 0xFF) {
                        Pixel current = get_pixel(x_out + dest.x_pos, y_out + dest.y_pos);
                        pixel.rgba.blend(current.rgba);
                    }

                    set_frame_pixel(x_out + dest.x_pos, y_out + dest.y_pos, pixel.value);
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

        // glyphs are rendered from top to bottom, so we need to offset smaller glyphs
        int render_from = font.get_font_size() - src.height;
        Rect dest = src;
        dest.x_pos = dest_x_pos;
        dest.y_pos = point.y + render_from;
        dest.width = glyph.width;
        dest.height = glyph.height;

        render_texture(*glyph.texture, src, dest);
        dest_x_pos += glyph.advance;
    }
}

