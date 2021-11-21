#include "Renderer.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>


Renderer::Renderer(GWindow* window) : p_window(window) {
    m_gc = XCreateGC(window->get_display(), window->get_window(), 0, nullptr);
    auto white_color = XWhitePixel(p_window->get_display(), p_window->get_screen_num());
    auto black_color = XBlackPixel(p_window->get_display(), p_window->get_screen_num());

    XSetBackground(p_window->get_display(), m_gc, black_color);
    XSetForeground(p_window->get_display(), m_gc, white_color);
    XSetFillStyle(window->get_display(), m_gc, FillSolid);
    XSetLineAttributes(window->get_display(), m_gc, 2, LineSolid, CapRound, JoinRound);

    Visual* default_visual = DefaultVisual(window->get_display(), DefaultScreen(window->get_display()));
    m_colormap = XCreateColormap(window->get_display(), window->get_window(), default_visual, AllocNone);

    XMatchVisualInfo(p_window->get_display(), p_window->get_screen_num(), 32, DirectColor, &m_visual_info);
    p_visual = m_visual_info.visual;


    m_framebuffer = new int[m_width * m_height];
    for (auto i = 0u; i < m_width * m_height; ++i) {
		*(m_framebuffer+i) = 0x000000ff;
	}

    setup_shared_memory();

    XSync(window->get_display(), false);
}

Renderer::~Renderer() {
    XFreeColormap(p_window->get_display(), m_colormap);
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

void Renderer::render() {

    // copy all the data from the frame_buffer to the shm buffer
    for (auto i = 0u; i < m_width *m_height; i++) {
		*((int*) m_shm_info.shmaddr+i) = *(m_framebuffer + 1);
	}

    Status status = XShmPutImage(p_window->get_display(), p_window->get_window(), m_gc, p_screen_image, 0, 0, 0, 0, m_width, m_height, true);

    if (status == 0) {
        printf("Something went wrong with rendering the shm Image\n");
    }
}

void Renderer::set_background_color(const Color &color) {
    XColor x_color;
    x_color.red = color.r;
    x_color.green = color.g;
    x_color.blue = color.b;

    Status rc = XAllocColor(p_window->get_display(), m_colormap, &x_color);
    XSetBackground(p_window->get_display(), m_gc, x_color.pixel);

    if (rc == 0) {
        printf("Failed to allocate color\n");
    }

    XSync(p_window->get_display(), false);
}

bool Renderer::setup_shared_memory() {
    auto shm_available = XShmQueryExtension(p_window->get_display());
    if (shm_available == 0) {
        printf("Shared memory not available\n");
        return false;
    }

    p_screen_image = XShmCreateImage(p_window->get_display(), nullptr, 24, ZPixmap, nullptr, &m_shm_info, m_width, m_height);

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
