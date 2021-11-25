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

void Renderer::draw_triangle(const Triangle &triangle) {
    Vertex min_y_vert = triangle.p[0];
	Vertex mid_y_vert = triangle.p[1];
	Vertex max_y_vert = triangle.p[2];

	if(max_y_vert.pos.y < mid_y_vert.pos.y)
	{
		Vertex temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

	if(mid_y_vert.pos.y < min_y_vert.pos.y)
	{
		Vertex temp = mid_y_vert;
		mid_y_vert = min_y_vert;
		min_y_vert = temp;
	}

	if(max_y_vert.pos.y < mid_y_vert.pos.y)
	{
		Vertex temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

    float x1 = max_y_vert.pos.x - min_y_vert.pos.x;
    float y1 = max_y_vert.pos.y - min_y_vert.pos.y;

    float x2 = mid_y_vert.pos.x - min_y_vert.pos.x;
    float y2 = mid_y_vert.pos.y - min_y_vert.pos.y;

    bool handedness =  (x1 * y2 - x2 * y1) >= 0.0f;

	scan_triangle(min_y_vert, mid_y_vert, max_y_vert, handedness);
}

void Renderer::clear_screen() {
    for (auto i = 0u; i < m_width * m_height; ++i) {
		*(m_framebuffer+i) = 0x00000000;
	}
}

void Renderer::scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness) {
  Gradients gradients = Gradients(min_y_vert, mid_y_vert, max_y_vert);

    Edge bottom_to_top = Edge(min_y_vert, max_y_vert, gradients, 0);
    Edge bottom_to_middle = Edge(min_y_vert, mid_y_vert, gradients, 0);
    Edge middle_to_top = Edge(mid_y_vert, max_y_vert, gradients, 1);

    scan_edges(bottom_to_top, bottom_to_middle, handedness, gradients);
    scan_edges(bottom_to_top, middle_to_top, handedness, gradients);
}

void Renderer::scan_edges(Edge &a, Edge &b, bool handedness, const Gradients &gradients) {
    int y_start = b.y_start;
    int y_end   = b.y_end;

    if (handedness) {
        for(int j = y_start; j < y_end; j++)
        {
            draw_scanline(b, a, j, gradients);
            a.step();
            b.step();
        }
    } else {
        for(int j = y_start; j < y_end; j++)
        {
            draw_scanline(a, b, j, gradients);
            a.step();
            b.step();
        }
    }
}

void Renderer::draw_scanline(const Edge &left, const Edge &right, int j, const Gradients &gradients) {
    int x_min = (int)std::ceil(left.x);
    int x_max = (int)std::ceil(right.x);

    float x_prestep = (float)x_min - left.x;

    Color color = left.color + gradients.x_step * x_prestep;

    for(int i = x_min; i < x_max; i++)
    {
        *(m_framebuffer + ((m_width * j) + i)) = color.to_uint32();
        color += gradients.x_step;
    }
}
