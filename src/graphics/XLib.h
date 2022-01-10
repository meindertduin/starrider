#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include "../core/Events.h"

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

typedef struct {
    unsigned int* buffer;
    int w, h;
} XScreen;

extern XWindow x_window;
extern XScreen x_screen;

void xlib_init(int cols, int rows);
void xlib_quit();

bool xpoll_event(WindowEvent &event);
void xinit_shm(int cols, int rows);

void xresize_window(int width, int height);

int xset_fullscreen_mode();
int xset_normal_mode();
int xset_float_mode();

void xrender_screen();

void xset_empty_cursor();
void xset_cursor_pos(int x_pos, int y_pos);

typedef struct {
    int win_x;
    int win_y;
    int screen_x;
    int screen_y;
} XCursorPos;

XCursorPos xquery_cursor_pos();
