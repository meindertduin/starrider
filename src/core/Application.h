#pragma once

#include "Window.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application {
public:
    ~Application();

    static Application* get_instance();

    bool initialize(const AppSettings &settings);
    void run();
    GWindow get_window();
protected:
    Application();

private:
    GWindow m_window;
    bool m_running;

    static Application *sp_instance;
};
