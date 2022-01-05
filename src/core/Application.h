#pragma once

#include "Window.h"
#include "Events.h"
#include "../graphics/Camera.h"
#include <memory>

#include "Cursor.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application : public MultiEventSubject<WindowEvent> {
public:
    ~Application();

    static std::shared_ptr<Application> get_instance();
    GWindow* get_window();
    Core::Cursor* get_cursor();

    bool initialize(const AppSettings &settings);
    void run();
    void send_window_event(WindowEvent event);
protected:
    Application();

private:
    GWindow m_window;
    Core::Cursor m_cursor;
    std::unique_ptr<Camera> p_camera { nullptr };
    bool m_running;
    int m_fps;

    void poll_window_events();
    void emit_mouse_motion_event();

    static std::shared_ptr<Application> sp_instance;
};
