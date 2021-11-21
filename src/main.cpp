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
        XNextEvent(window.get_display(), &event);

        if (event.type == Expose)  {
           renderer.render();
        }

        if (event.type == KeyPress) {
            break;
        }
    }

    return 0;
}

