#pragma once

#include "Window.h"
#include "Events.h"
#include "../graphics/Camera.h"

#include "Cursor.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application : public MultiEventSubject<WindowEvent> {
public:
    static Application* get_instance();
    ~Application();

    bool initialize(const AppSettings &settings);
    void run();
    GWindow* get_window();
    Core::Cursor* get_cursor();

    void send_window_event(WindowEvent event);
protected:
    Application();

private:
    GWindow m_window;
    Core::Cursor m_cursor;
    Camera *p_camera = nullptr;
    bool m_running;
    int m_fps;

    void poll_window_events();

    static Application *sp_instance;
};
