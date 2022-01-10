#pragma once

#include <stdint.h>

class GWindow;

namespace Core {

class Cursor {
public:
    Cursor();

    struct MouseMovement{
        uint32_t x_pos : 16;
        uint32_t y_pos : 16;
        int delta_x : 16;
        int delta_y : 16;
    };

    void initialize(GWindow *root_window);
    void reset_pos_middle();
    MouseMovement get_delta_movement();
private:
    GWindow *p_window;
    uint32_t m_xpos, m_ypos;

    void set_empty_cursor();
};

}

