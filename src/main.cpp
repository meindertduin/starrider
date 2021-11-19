#include <stdio.h>
#include <X11/Xlib.h>

int main() {
    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        return 1;
    }

    screen = DefaultScreen(display);

    int black_color = BlackPixel(display, screen);
    int white_color = WhitePixel(display, screen);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 200, 100, 0, black_color, black_color);
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display , window);

    GC gc = XCreateGC(display, window, 0, nullptr);

    for (;;) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            XSetForeground(display, gc, white_color);
            XFillRectangle(display, window, gc, 20, 20, 10, 10);
            XDrawLine(display, window, gc, 50, 50, 100, 100);
            XFlush(display);
        }

        if (event.type == KeyPress) {
            break;
        }
    }

    XCloseDisplay(display);
    return 0;
}
