#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include "../core/Events.h"

void xlib_init(int cols, int rows);
void xlib_quit();

bool poll_event(WindowEvent &event);
void xinit_shm(int cols, int rows);
void resize(int width, int height);

void set_fullscreen_mode();
void set_normal_mode();
void set_float_mode();
