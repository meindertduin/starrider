#include "Cursor.h"
#include "Window.h"

#include <stdio.h>

namespace Core {

Cursor::Cursor() {

}

Cursor::~Cursor() {
    if (m_cursor)
        XFreeCursor(p_window->get_display(), m_cursor);
}

void Cursor::reset_pos_middle() {
    m_xpos = static_cast<int>((float)p_window->m_width / 2.0f);
    m_ypos = static_cast<int>((float)p_window->m_height / 2.0f);

    XWarpPointer(p_window->get_display(), None, p_window->get_window(), 0, 0,
        0, 0, m_xpos, m_ypos);
}

void Cursor::initialize(GWindow *root_window) {
    p_window = root_window;
    set_empty_cursor();
}

Cursor::MouseMovement Cursor::get_delta_movement() {
    int x_pos, y_pos, win_x, win_y;
    unsigned int mask;

    Window root, child;
    if (!XQueryPointer(p_window->get_display(), p_window->get_window(), &root, &child, &x_pos, &y_pos, &win_x, &win_y, &mask)) {
        printf("Couldn't query pointer position\n");
    }

    return {
        .x_pos = m_xpos,
        .y_pos = m_ypos,
        .delta_x = win_x - static_cast<int>(m_xpos),
        .delta_y = win_y - static_cast<int>(m_ypos),
    };
}

void Cursor::set_empty_cursor() {
    XColor color  = { 0 };
    const char data[] = { 0 };

    Pixmap pixmap = XCreateBitmapFromData(p_window->get_display(), p_window->get_window(), data, 1, 1);

    if (m_cursor) {
        XFreeCursor(p_window->get_display(), m_cursor);
        m_cursor = 0;
    }

    m_cursor = XCreatePixmapCursor(p_window->get_display(), pixmap, pixmap, &color, &color, 0, 0);

    XFreePixmap(p_window->get_display(), pixmap);
    XDefineCursor(p_window->get_display(), p_window->get_window(), m_cursor);
}

}

