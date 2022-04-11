#pragma once

#include "Window.h"
#include "Events.h"
#include "../graphics/Camera.h"
#include "../graphics/RenderObject.h"
#include <memory>

#include "Cursor.h"

struct AppSettings {
    int win_width;
    int win_height;
};

class Application : public MultiEventSubject<WindowEvent> {
public:
    ~Application();
    Application(const Application &other) = delete;
    Application(Application &&other) = delete;

    Application& operator=(const Application &other) = delete;
    Application& operator=(Application &&other) = delete;

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
    std::unique_ptr<Graphics::Camera> p_camera { nullptr };
    bool m_running;
    int m_fps;

    Graphics::RenderContext m_rc;

    void poll_window_events();
    void emit_mouse_motion_event();

    static std::shared_ptr<Application> sp_instance;
};
