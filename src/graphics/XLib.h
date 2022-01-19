#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include "../core/Events.h"

namespace XLib {

typedef struct {
    unsigned int* buffer;
    int width, height;
} XScreen;

extern XScreen x_screen;

void lib_init(int cols, int rows, int border_width);
void lib_quit();

bool poll_event(WindowEvent &event);
void init_shm(int cols, int rows);

void resize_window(int width, int height);

int set_win_fullscreen_mode();
int set_win_normal_mode();
int set_win_float_mode();

bool render_screen();

void set_empty_cursor();
void set_cursor_pos(int x_pos, int y_pos);

typedef struct {
    int win_x;
    int win_y;
    int screen_x;
    int screen_y;
} XCursorPos;

XCursorPos query_cursor_pos();

}

