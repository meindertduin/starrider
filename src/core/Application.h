#pragma once

#include "Window.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application {
public:
    Application();
    bool initialize(const AppSettings &settings);

    GWindow get_window();
    void run();
private:
    GWindow m_window;
    bool m_running;
};
