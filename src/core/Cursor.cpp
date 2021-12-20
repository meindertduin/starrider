#include "Cursor.h"
#include "Window.h"

namespace Core {

Cursor::Cursor() {

}

Cursor::~Cursor() {
    XFreeCursor(p_window->get_display(), m_cursor);
}

void Cursor::reset_pos_middle() {
    XWarpPointer(p_window->get_display(), None, p_window->get_window(), 0, 0,
        0, 0, (float)p_window->m_width / 2.0f, (float)p_window->m_height / 2.0f);
}

void Cursor::initialize(GWindow *root_window) {
    p_window = root_window;
    m_cursor = XCreateFontCursor(root_window->get_display(), 2);
}

}

