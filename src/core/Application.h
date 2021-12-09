#pragma once

#include "Window.h"
#include "Events.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application : public EventSubject<InputEvent> {
public:
    static Application* get_instance();
    ~Application();

    bool initialize(const AppSettings &settings);
    void run();
    GWindow get_window();
protected:
    Application();

private:
    GWindow m_window;
    bool m_running;

    void poll_window_events();

    static Application *sp_instance;
};
