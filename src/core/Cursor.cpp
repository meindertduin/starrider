#include "Cursor.h"
#include "Window.h"

#include <stdio.h>
#include "../graphics/XLib.h"

namespace Core {

Cursor::Cursor() {

}

void Cursor::reset_pos_middle() {
    m_xpos = static_cast<int>((float)p_window->get_width() / 2.0f);
    m_ypos = static_cast<int>((float)p_window->get_height() / 2.0f);

    xset_cursor_pos(m_xpos, m_ypos);
}

void Cursor::initialize(GWindow *root_window) {
    p_window = root_window;
    set_empty_cursor();
}

Cursor::MouseMovement Cursor::get_delta_movement() {
    auto cursor_pos = xquery_cursor_pos();

    return {
        .x_pos = m_xpos,
        .y_pos = m_ypos,
        .delta_x = cursor_pos.win_x - static_cast<int>(m_xpos),
        .delta_y = cursor_pos.win_y - static_cast<int>(m_ypos),
    };
}

void Cursor::set_empty_cursor() {
    xset_empty_cursor();
}

}

