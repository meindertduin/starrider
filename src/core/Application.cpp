#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/RenderPipeline.h"
#include "../graphics/Texture.h"

#include "KeyMap.h"
#include "Time.h"

Application* Application::sp_instance = nullptr;

Application::Application() {
    m_fps = 60;
}

Application::~Application() {
    delete sp_instance;

    if (p_camera != nullptr)
        delete p_camera;
}

Application* Application::get_instance() {
    if (sp_instance == nullptr) {
        sp_instance = new Application();
    }

    return sp_instance;
}

bool Application::initialize(const AppSettings &settings) {
    bool success = m_window.initialize(settings.win_width, settings.win_height);
    if (!success) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return false;
    }

    return true;
}

void Application::run() {
    m_running = true;

    p_camera = new Camera();
    p_camera->set_viewport(m_window.m_width, m_window.m_height);

    m_cursor.initialize(&m_window);

    Texture brick_texture;
    brick_texture.load_from_bmp("test_texture.bmp");

    Renderer renderer;
    RenderPipeline render_pipeline {&renderer};

    // TODO: Values that may belong to a scene?
    Mesh mesh;
    mesh.load_from_obj_file("monkey0.obj");
    mesh.texture = &brick_texture;

    Transform monkey_transform = Transform(V4F(0, 0, 3));

    Matrix4F vp = p_camera->get_view_projection();
    // m_window.toggle_fullscreen();

    while (m_running) {
        m_cursor.reset_pos_middle();

        auto cycle_start = get_program_ticks_ms();

        poll_window_events();

        monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5f));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

       render_pipeline.render_viewport(*p_camera, renderables);
       renderer.render();

        auto dt = get_program_ticks_ms() - cycle_start;
        int cycle_delay = (1000.0f / (float)m_fps) - dt;
        if (cycle_delay > 0) {
            delay(cycle_delay);
        }

        emit_mouse_motion_event();
    }
}

void Application::emit_mouse_motion_event() {
    auto delta_mouse = m_cursor.get_delta_movement();
    WindowEvent mouse_motion;
    mouse_motion.event_type = WindowEventType::MouseMotion;
    mouse_motion.body.mouse_event = {
        .x_pos = delta_mouse.x_pos,
        .y_pos = delta_mouse.y_pos,
        .d_xpos = delta_mouse.delta_x,
        .d_ypos = delta_mouse.delta_y,
    };

    emit_event(mouse_motion, mouse_motion.event_type);
}

void Application::poll_window_events() {
    WindowEvent event;

    while(m_window.poll_event(event)) {
        switch(event.event_type) {
            case WindowEventType::WinExpose:
            {
                p_camera->set_viewport(event.body.expose_event.width, event.body.expose_event.height);

                emit_event(event, event.event_type);
            }
            break;
            case WindowEventType::KeyDown:
            {
                emit_event(event, event.event_type);
            }
            break;
            default:
                break;

        }

        event.event_type = WindowEventType::WinNone;
    }
}

GWindow* Application::get_window() {
    return &m_window;
}

Core::Cursor* Application::get_cursor() {
    return &m_cursor;
}

void Application::send_window_event(WindowEvent event) {
    emit_event(event, WindowEventType::WinExpose);
}
