#include "Cursor.h"
#include "Window.h"

#include <stdio.h>
#include "../platform/x11/XLib.h"

namespace Core {

Cursor::Cursor() {

}

void Cursor::reset_pos_middle() {
    m_xpos = p_window->get_width() / 2;
    m_ypos = p_window->get_height() / 2;

    XLib::set_cursor_pos(m_xpos, m_ypos);
}

void Cursor::initialize(GWindow *root_window) {
    p_window = root_window;
    set_empty_cursor();
}

Cursor::MouseMovement Cursor::get_delta_movement() {
    auto cursor_pos = XLib::query_cursor_pos();

    MouseMovement result = {
        .x_pos = m_xpos,
        .y_pos = m_ypos,
        .delta_x = cursor_pos.win_x - static_cast<int>(m_xpos),
        .delta_y = cursor_pos.win_y - static_cast<int>(m_ypos),
    };

    return result;
}

void Cursor::set_empty_cursor() {
    XLib::set_empty_cursor();
}

}

