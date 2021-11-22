#include <stdio.h>
#include <X11/Xlib.h>

#include "graphics/Renderer.h"

int main() {
    GWindow window;
    if (!window.initialize()) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return 1;
    }

    Renderer renderer {&window};

    Triangle tri;

    tri.p[0] = V3F(10, 10, 0);
    tri.p[1] = V3F(200, 10, 0);
    tri.p[2] = V3F(10, 300, 0);

    XEvent event;
    for (;;) {
        while(window.poll_event(event)) {
            if (event.type == Expose)  {
            }
        }

        renderer.draw_line({ 100, 100 }, { 400, 400 }, { 0x00, 0xFF, 0x00 });
        renderer.draw_triangle(tri);
        renderer.render();
    }

    return 0;
}

