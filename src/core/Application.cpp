#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/RenderPipeline.h"

#include "KeyMap.h"

Application* Application::sp_instance = nullptr;

Application::Application() {
    m_camera.set_viewport(m_window.m_width, m_window.m_height);
}

Application::~Application() {
    delete sp_instance;
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

    Bitmap texture("test_texture.bmp");
    Renderer renderer;
    RenderPipeline render_pipeline {&renderer};

    // TODO: Values that may belong to a scene?
    Mesh mesh;
    mesh.load_from_obj_file("monkey0.obj");
    mesh.texture = &texture;

    Transform monkey_transform = Transform(V4F(0, 0, 3));

    Matrix4F vp = m_camera.get_view_projection();
    //m_window.toggle_fullscreen();

    float time = 0.0f;
    while (m_running) {
        poll_window_events();

        monkey_transform = monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

        render_pipeline.render_frame(m_camera, renderables);

        time += 1.0f / 300.0f;
    }
}

void Application::poll_window_events() {
    WindowEvent event;

    while(m_window.poll_event(event)) {
        // resize from the server event
        switch(event.event_type) {
            case WindowEventType::WinExpose:
            {
                m_camera.set_viewport(event.body.expose_event.width, event.body.expose_event.height);

                emit_event(event, event.event_type);
            }
            break;
            case WindowEventType::KeyDown:
            {

            }
            break;
            default:
                break;

        }
    }
}

GWindow* Application::get_window() {
    return &m_window;
}

void Application::send_window_event(WindowEvent event) {
    emit_event(event, WindowEventType::WinExpose);
}
