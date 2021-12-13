#pragma once

#include "Window.h"
#include "Events.h"
#include "../graphics/Camera.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application : public MultiEventSubject<InputEvent> {
public:
    static Application* get_instance();
    ~Application();

    bool initialize(const AppSettings &settings);
    void run();
    GWindow* get_window();
    void send_window_event(InputEvent event);
protected:
    Application();

private:
    GWindow m_window;
    Camera m_camera;
    bool m_running;

    void poll_window_events();

    static Application *sp_instance;
};
