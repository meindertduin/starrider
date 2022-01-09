#include "XLib.h"

#include <X11/Xutil.h>

typedef struct {
    int w, h;
    Display *display;
    Colormap color_map;
    Window win;
    Visual *vis;
    XSetWindowAttributes attrs;
    int screen;
    int depth; /* bit depth */
} XWindow;

static XWindow x_window;

void xlib_init(int cols, int rows) {
    XWindowAttributes attr;
    Window parent;
    XVisualInfo vis;

    x_window.display = XOpenDisplay(NULL);

    if (x_window.display == nullptr) {
        // TODO add error handling
    }

    parent = XDefaultRootWindow(x_window.display);

    x_window.screen = DefaultScreen(x_window.display);

    XGetWindowAttributes(x_window.display, parent, &attr);
    x_window.depth = attr.depth;

    XMatchVisualInfo(x_window.display, x_window.screen, x_window.depth, TrueColor, &vis);
    x_window.vis = vis.visual;

    x_window.color_map = XCreateColormap(x_window.display, parent, x_window.vis, None);

    x_window.attrs.background_pixel = BlackPixel(x_window.display, x_window.screen);
    x_window.attrs.border_pixel = 0;
    x_window.attrs.bit_gravity = NorthWestGravity;
    x_window.attrs.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

    x_window.win = XCreateWindow(x_window.display, parent, 0, 0,
             cols, rows, 0, x_window.depth, InputOutput,
             x_window.vis, CWBackPixel | CWBorderPixel | CWBitGravity
             | CWEventMask | CWColormap, &x_window.attrs);

    // add the events
    XSelectInput(x_window.display, x_window.win, x_window.attrs.event_mask);

    XMapWindow(x_window.display , x_window.win);
    XSync(x_window.display, False);
}

