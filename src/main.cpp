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

    XEvent event;
    for (;;) {
        window.poll_event(event);

        if (event.type == Expose)  {
            renderer.draw_line({ 20, 20 }, { 400, 400 }, { 0x00, 0xFF, 0x00 });
            renderer.render();
        }

    }

    return 0;
}

