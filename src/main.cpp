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
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display , window);

    GC gc = XCreateGC(display, window, 0, nullptr);
    XSetForeground(display, gc, white_color);
    XSetBackground(display, gc,  black_color);
    XSetFillStyle(display, gc, FillSolid);
    XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);
    XSync(display, false);

    for (;;) {
        XNextEvent(display, &event);

        if (event.type == ButtonPress) {
            int x = event.xbutton.x;
            int y = event.xbutton.y;

            switch (event.xbutton.button) {
                case Button1:
                    XDrawPoint(display, window, gc, x, y);
                    break;
                default:
                    break;
            }
        }

        if (event.type == Expose) {
            XFillRectangle(display, window, gc, 20, 20, 40, 40);
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
