#pragma once

#include "Window.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application {
public:
    static Application* get_instance();

    bool initialize(const AppSettings &settings);
    void run();
    GWindow get_window();
protected:
    Application();
    ~Application();

private:
    GWindow m_window;
    bool m_running;

    void poll_window_events();

    static Application *sp_instance;
};
