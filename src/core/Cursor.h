#pragma once

#include <X11/Xlib.h>

typedef Cursor XCursor;
class GWindow;

namespace Core {

class Cursor {
public:
    Cursor();
    ~Cursor();

    void initialize(GWindow *root_window);
    void reset_pos_middle();
private:
    XCursor m_cursor;
    GWindow *p_window;
};

}

