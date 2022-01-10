#include "XLib.h"

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

typedef Cursor XCursor;

typedef struct {
    XImage* screen_image;
    GC gc;
} XDisplay;



XWindow x_window;
XScreen x_screen;

static XDisplay x_display;
static XShmSegmentInfo shm_info;
static XCursor cursor;

bool xsetup_shared_memory();
void xremove_shared_memory();

inline void xresize(int width, int height);

void xlib_quit() {
    if (x_window.display != nullptr)
        XCloseDisplay(x_window.display);

    if (x_display.screen_image != nullptr)
        XDestroyImage(x_display.screen_image);

    if (cursor)
        XFreeCursor(x_window.display, cursor);


    xremove_shared_memory();
}

void xlib_init(int width, int height) {
    XWindowAttributes attr;
    XVisualInfo vis;

    x_window.w = width;
    x_window.h = height;

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
             width, height, 0, x_window.depth, InputOutput,
             x_window.vis, CWBackPixel | CWBorderPixel | CWBitGravity
             | CWEventMask | CWColormap, &x_window.attrs);

    // add the events
    XSelectInput(x_window.display, x_window.win, x_window.attrs.event_mask);

    xset_float_mode();
    xsetup_shared_memory();

    x_screen.buffer = reinterpret_cast<unsigned int*>(shm_info.shmaddr);

    // might need to be set different than window.w or h when padding is added
    x_screen.w = x_window.w;
    x_screen.h = x_window.h;

    x_display.gc = XCreateGC(x_window.display, x_window.win, 0, nullptr);
    auto black_color = XBlackPixel(x_window.display, x_window.screen);
    XSetBackground(x_window.display, x_display.gc, black_color);


    XMapWindow(x_window.display , x_window.win);
    XSync(x_window.display, False);
}

bool xpoll_event(WindowEvent &event) {
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

void xresize_window(int width, int height) {
    XResizeWindow(x_window.display, x_window.win, width, height);
    XMapWindow(x_window.display , x_window.win);

    xresize(width, height);
}

inline void xresize(int width, int height) {
    xremove_shared_memory();
    xsetup_shared_memory();

    x_window.w = width;
    x_window.h = height;

    x_screen.buffer = reinterpret_cast<unsigned int*>(shm_info.shmaddr);

    // might need to be set different than window.w or h when padding is added
    x_screen.w = x_window.w;
    x_screen.h = x_window.h;
}

int xset_float_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_dialog = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    auto status  =XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_dialog, 1);
    XResizeWindow(x_window.display, x_window.win, x_window.w, x_window.h);

    return status;
}

int xset_fullscreen_mode() {
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

    return XSendEvent(x_window.display, x_window.root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

int xset_normal_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_normal = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    return XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_normal, 1);
}

bool xsetup_shared_memory() {
    auto shm_available = XShmQueryExtension(x_window.display);
    if (shm_available == 0) {
        return false;
    }

    x_display.screen_image = XShmCreateImage(x_window.display, x_window.vis, 24, ZPixmap, nullptr, &shm_info, x_window.w, x_window.h);

    shm_info.shmid = shmget(IPC_PRIVATE, x_display.screen_image->bytes_per_line * x_display.screen_image->height, IPC_CREAT | 0777);
    if (shm_info.shmid == -1) {
        return false;
    }

    shm_info.shmaddr = x_display.screen_image->data = (char*) shmat(shm_info.shmid, nullptr, 0);
    shm_info.readOnly = false;

    auto shm_attach = XShmAttach(x_window.display, &shm_info);
    if (shm_attach == 0) {
        return false;
    }

    return true;
}

void xremove_shared_memory() {
    XShmDetach(x_window.display, &shm_info);
    shmdt(shm_info.shmaddr);
    shmctl(shm_info.shmid, IPC_RMID, 0);
}

void xrender_screen() {
    Status status = XShmPutImage(x_window.display, x_window.win, x_display.gc, x_display.screen_image, 0, 0, 0, 0, x_window.w, x_window.h, true);

    if (status == 0) {
        // TODO add error logging
    }
}

void xset_empty_cursor() {
    XColor color  = { 0 };
    const char data[] = { 0 };

    Pixmap pixmap = XCreateBitmapFromData(x_window.display, x_window.win, data, 1, 1);

    if (cursor) {
        XFreeCursor(x_window.display, cursor);
        cursor = 0;
    }

    cursor = XCreatePixmapCursor(x_window.display, pixmap, pixmap, &color, &color, 0, 0);

    XFreePixmap(x_window.display, pixmap);
    XDefineCursor(x_window.display, x_window.win, cursor);
}

void xset_cursor_pos(int x_pos, int y_pos) {
    XWarpPointer(x_window.display, None, x_window.win, 0, 0, 0, 0, x_pos, y_pos);
    XSync(x_window.display, False);
}

XCursorPos xquery_cursor_pos() {
    int x_pos, y_pos, win_x, win_y;
    unsigned int mask;

    Window root, child;
    if (!XQueryPointer(x_window.display, x_window.win, &root, &child, &x_pos, &y_pos, &win_x, &win_y, &mask)) {
        printf("Couldn't query pointer position\n");
    }

    return {
        .win_x = win_x,
        .win_y = win_y,
        .screen_x = x_pos,
        .screen_y = y_pos,
    };
}
