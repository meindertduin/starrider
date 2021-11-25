#include "Renderer.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>


Renderer::Renderer(GWindow* window) : p_window(window) {
    m_gc = XCreateGC(window->get_display(), window->get_window(), 0, nullptr);
    auto black_color = XBlackPixel(p_window->get_display(), p_window->get_screen_num());

    XSetBackground(p_window->get_display(), m_gc, black_color);

    p_visual = DefaultVisual(window->get_display(), DefaultScreen(window->get_display()));

    m_framebuffer = new uint32_t[m_width * m_height];
    for (auto i = 0u; i < m_width * m_height; ++i) {
		*(m_framebuffer+i) = 0x00000000;
	}

    setup_shared_memory();

    XSync(window->get_display(), false);
}

Renderer::~Renderer() {
    XShmDetach(p_window->get_display(), &m_shm_info);
    shmdt(m_shm_info.shmaddr);
    shmctl(m_shm_info.shmid, IPC_RMID, 0);

    XDestroyImage(p_screen_image);

    delete[] m_framebuffer;
    p_visual = nullptr;
    p_window = nullptr;
}

void Renderer::set_color(const Color &color) {

}

bool Renderer::render() {
    while (!p_window->ready_for_render) {
        // block untill drawing is complete
    }

    // copy all the data from the frame_buffer to the shm buffer
    for (auto i = 0u; i < m_width *m_height; i++) {
		*((int*) m_shm_info.shmaddr+i) = *(m_framebuffer + i);
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

void Renderer::draw_line(const Point &p1, const Point &p2, const Color &color) {
    if (p1.x > m_width || p1.y > m_height || p2.x > m_width || p2.y > m_height) {
        return;
    }

    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    u_int32_t p_code = color.to_uint32();

    if (dx == 0 && dy == 0) {
        m_framebuffer[m_width * p1.y + p1.x] = p_code;
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
            *(m_framebuffer + ((m_width * y) + x)) = p_code;
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
            *(m_framebuffer + ((m_width * y) + x)) = p_code;
        }
    }
}

void Renderer::clear_screen() {
    for (auto i = 0u; i < m_width * m_height; ++i) {
		*(m_framebuffer+i) = 0x00000000;
	}
}

void Renderer::set_frame_pixel(int x_pos, int y_pos, uint32_t value) {
    *(m_framebuffer + ((m_width * y_pos) + x_pos)) = value;
}
