#include "XLib.h"

#include <X11/Xutil.h>
#include <X11/Xatom.h>

typedef struct {
    int w, h;
    Display *display;
    Colormap color_map;
    Window win;
    Visual *vis;
    XSetWindowAttributes attrs;
    int screen;
    int depth; /* bit depth */
    Window root;
} XWindow;

static XWindow x_window;

void xlib_quit() {
    if (x_window.display != nullptr)
        XCloseDisplay(x_window.display);
}

void xlib_init(int cols, int rows) {
    XWindowAttributes attr;
    XVisualInfo vis;

    x_window.w = cols;
    x_window.h = rows;

    x_window.display = XOpenDisplay(NULL);

    if (x_window.display == nullptr) {
        // TODO add error handling
    }

    x_window.root = XDefaultRootWindow(x_window.display);
    x_window.screen = DefaultScreen(x_window.display);

    XGetWindowAttributes(x_window.display, x_window.root, &attr);
    x_window.depth = attr.depth;

    XMatchVisualInfo(x_window.display, x_window.screen, x_window.depth, TrueColor, &vis);
    x_window.vis = vis.visual;

    x_window.color_map = XCreateColormap(x_window.display, x_window.root, x_window.vis, None);

    x_window.attrs.background_pixel = BlackPixel(x_window.display, x_window.screen);
    x_window.attrs.border_pixel = 0;
    x_window.attrs.bit_gravity = NorthWestGravity;
    x_window.attrs.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

    x_window.win = XCreateWindow(x_window.display, x_window.root, 0, 0,
             cols, rows, 0, x_window.depth, InputOutput,
             x_window.vis, CWBackPixel | CWBorderPixel | CWBitGravity
             | CWEventMask | CWColormap, &x_window.attrs);

    // add the events
    XSelectInput(x_window.display, x_window.win, x_window.attrs.event_mask);

    set_float_mode();

    XMapWindow(x_window.display , x_window.win);
    XSync(x_window.display, False);
}

bool poll_event(WindowEvent &event) {
    if (XPending(x_window.display)) {
        XEvent x_event;
        XNextEvent(x_window.display, &x_event);

        switch(x_event.type) {
            case Expose:
                XWindowAttributes attributes;
                XGetWindowAttributes(x_window.display, x_window.win, &attributes);

                if (attributes.width == x_window.w && attributes.height == x_window.h) {
                    return false;
                }

                x_window.w = attributes.width;
                x_window.h = attributes.height;

                event.body.expose_event.width = x_window.w;
                event.body.expose_event.height = x_window.h;

                event.event_type = WindowEventType::WinExpose;

                break;
            case KeyPress:
                event.body.keyboard_event.keysym = XLookupKeysym(&x_event.xkey, 0);
                event.body.keyboard_event.mask = x_event.xkey.state;

                event.event_type = WindowEventType::KeyDown;
                break;
            default:
                break;
        }

        return true;
    }

    return false;
}

void resize(int width, int height) {
    XResizeWindow(x_window.display, x_window.win, width, height);
    XMapWindow(x_window.display , x_window.win);

    x_window.w = width;
    x_window.h = height;
}

void set_float_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_dialog = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    auto status  =XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_dialog, 1);
    XResizeWindow(x_window.display, x_window.win, x_window.w, x_window.h);
}

void set_fullscreen_mode() {
    Atom wm_state_atom = XInternAtom(x_window.display, "_NET_WM_STATE", False);
    Atom fullscreen_atom = XInternAtom(x_window.display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = x_window.win;
    xev.xclient.message_type = wm_state_atom;
    xev.xclient.format= 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen_atom;
    xev.xclient.data.l[2] = 0;

    auto status = XSendEvent(x_window.display, x_window.root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

void set_wind_normal_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_normal = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    auto status = XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_normal, 1);
}
