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

    m_framebuffer = new int[m_width * m_height];
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

    u_int32_t p_code = get_pixel_code(color);

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

u_int32_t Renderer::get_pixel_code(const Color &color) {
    return 0x00000000 | (color.r << 16) | (color.g << 8) | color.b;
}

void Renderer::draw_triangle(const Triangle &triangle) {
    Edge edges[3];

    edges[0] = Edge(triangle.p[0], triangle.p[1]);
    edges[1] = Edge(triangle.p[1], triangle.p[2]);
    edges[2] = Edge(triangle.p[2], triangle.p[0]);

    int max_len = 0;
    int long_edge = 0;

    for (auto i = 0u; i < 3; i++) {
       int length = edges[i].p[1].y - edges[i].p[0].y;

        if (length > max_len) {
            max_len = length;
            long_edge = i;
        }
    }

    int short_edge1 = (long_edge + 1) % 3;
    int short_edge2 = (long_edge + 2) % 3;
    draw_between_edges(edges[long_edge], edges[short_edge1]);
    draw_between_edges(edges[long_edge], edges[short_edge2]);
}

void Renderer::draw_between_edges(const Edge &e1, const Edge &e2) {
    float e1dy = (float) (e1.p[1].y - e1.p[0].y);
    if (e1dy == 0.0f) return;

    float e2dy = (float) (e2.p[1].y - e2.p[0].y);
    if (e2dy == 0.0f) return;

    float e1dx = (float) (e1.p[1].x - e1.p[0].x);
    float e2dx = (float) (e2.p[1].x - e2.p[0].x);

    float f1 = (float) (e2.p[0].y - e1.p[0].y) / e1dy;
    float fstep1 = 1.0f / e1dy;

    float f2 = 0.0f;
    float fstep2 = 1.0f / e2dy;

    for (int y = e2.p[0].y; y < e2.p[1].y; y++) {
        auto span = Span(e1.p[0].x + (int) (e1dx * f1), e2.p[0].x + (int)(e2dx * f2));

        draw_span(span, y);

        f1 += fstep1;
        f2 += fstep2;
    }
}

void Renderer::draw_span(const Span &span, const int &y) {
    int dx = span.x2 - span.x1;
    if (dx == 0) {
        return;
    }

    for (auto x = span.x1; x < span.x2; x++) {
        *(m_framebuffer + ((m_width * y) + x)) = 0xffffffff;
    }
}
