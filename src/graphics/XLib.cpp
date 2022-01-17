#include "XLib.h"

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdexcept>

typedef Cursor XCursor;

typedef struct {
    int border_width;
    Display *display;
    Window win;
    Visual *vis;
    XSetWindowAttributes attrs;
    int screen;
    int depth; /* bit depth, usally 24 */
    Window root;
} XWindow;

typedef struct {
    XImage* screen_image;
    GC gc;
} XDisplay;

namespace XLib {

XScreen x_screen;

static XWindow x_window;
static XDisplay x_display;
static XShmSegmentInfo shm_info;
static XCursor cursor;

bool setup_shared_memory();
void remove_shared_memory();

inline void resize(int width, int height);

void lib_quit() {
    if (x_window.display != nullptr)
        XCloseDisplay(x_window.display);

    if (x_display.screen_image != nullptr)
        XDestroyImage(x_display.screen_image);

    if (cursor)
        XFreeCursor(x_window.display, cursor);


    remove_shared_memory();
}

void lib_init(int width, int height, int border_width) {
    XWindowAttributes attr;
    XVisualInfo vis;

    x_screen.width = width + border_width * 2;
    x_screen.height = height + border_width * 2;
    x_window.border_width = border_width;

    // creating display and setting defaults
    x_window.display = XOpenDisplay(NULL);

    if (x_window.display == nullptr)
        throw std::runtime_error("Display could not be opened");

    x_window.root = XDefaultRootWindow(x_window.display);
    x_window.screen = DefaultScreen(x_window.display);

    // Collecting window attributes
    XGetWindowAttributes(x_window.display, x_window.root, &attr);
    x_window.depth = attr.depth;

    XMatchVisualInfo(x_window.display, x_window.screen, x_window.depth, TrueColor, &vis);
    x_window.vis = vis.visual;

    // adding events
    x_window.attrs.background_pixel = BlackPixel(x_window.display, x_window.screen);
    x_window.attrs.border_pixel = WhitePixel(x_window.display, x_window.screen);
    x_window.attrs.bit_gravity = NorthWestGravity;
    x_window.attrs.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

    // window creation
    x_window.win = XCreateWindow(x_window.display, x_window.root, 0, 0,
             width, height, border_width, x_window.depth, InputOutput,
             x_window.vis, CWBackPixel | CWBorderPixel | CWBitGravity | CWEventMask | CWColormap,
             &x_window.attrs);

    set_win_float_mode();
    setup_shared_memory();

    x_screen.buffer = reinterpret_cast<unsigned int*>(shm_info.shmaddr);

    // might need to be set different than window.w or h when padding is added
    x_screen.width = x_screen.width;
    x_screen.height = x_screen.height;

    x_display.gc = XCreateGC(x_window.display, x_window.win, 0, nullptr);

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

                if (attributes.width == x_screen.width && attributes.height == x_screen.height) {
                    return false;
                }

                x_screen.width = attributes.width;
                x_screen.height = attributes.height;

                event.body.expose_event.width = x_screen.width;
                event.body.expose_event.height = x_screen.height;

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

void resize_window(int width, int height) {
    XResizeWindow(x_window.display, x_window.win, width, height);
    XMapWindow(x_window.display , x_window.win);

    resize(width, height);
}

inline void resize(int width, int height) {
    remove_shared_memory();

    if (!setup_shared_memory())
        throw std::runtime_error("Could not setup shared_memory");

    x_screen.width = width;
    x_screen.height = height;

    x_screen.buffer = reinterpret_cast<unsigned int*>(shm_info.shmaddr);

    // might need to be set different than window.w or h when padding is added
    x_screen.width = x_screen.width;
    x_screen.height = x_screen.height;
}

int set_win_float_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_dialog = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    auto status  =XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_dialog, 1);
    XResizeWindow(x_window.display, x_window.win, x_screen.width, x_screen.height);

    return status;
}

int set_win_fullscreen_mode() {
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

int set_win_normal_mode() {
    Atom window_type = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_normal = XInternAtom(x_window.display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    return XChangeProperty(x_window.display, x_window.win, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*) &type_normal, 1);
}

bool setup_shared_memory() {
    auto shm_available = XShmQueryExtension(x_window.display);
    if (shm_available == 0) {
        return false;
    }

    x_display.screen_image = XShmCreateImage(x_window.display, x_window.vis, 24, ZPixmap, nullptr, &shm_info, x_screen.width, x_screen.height);

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

void remove_shared_memory() {
    XShmDetach(x_window.display, &shm_info);
    shmdt(shm_info.shmaddr);
    shmctl(shm_info.shmid, IPC_RMID, 0);
}

void render_screen() {
    Status status = XShmPutImage(x_window.display, x_window.win, x_display.gc, x_display.screen_image, 0, 0, 0, 0, x_screen.width, x_screen.height, true);

    if (status == 0) {
        // TODO add error logging
    }
}

void set_empty_cursor() {
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

void set_cursor_pos(int x_pos, int y_pos) {
    XWarpPointer(x_window.display, None, x_window.win, 0, 0, 0, 0, x_pos, y_pos);
    XSync(x_window.display, False);
}

XCursorPos query_cursor_pos() {
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
}
