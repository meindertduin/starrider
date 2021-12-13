#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/RenderPipeline.h"
#include "../math/Radians.h"

Application* Application::sp_instance = nullptr;

Application::Application() {
    Matrix4F projection;
    projection.init_perspective(deg_to_half_rad(90.0f), 1.0f, 0.1f, 1000.0f);

    p_camera = new Camera(projection);

    p_camera->width = m_window.m_width;
    p_camera->height = m_window.m_height;
}

Application::~Application() {
    delete sp_instance;
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

    Bitmap texture("test_texture.bmp");
    Renderer renderer;
    RenderPipeline render_pipeline {&renderer};

    // TODO: Values that may belong to a scene?
    Mesh mesh;
    mesh.load_from_obj_file("monkey0.obj");
    mesh.texture = &texture;

    Transform monkey_transform = Transform(V4F(0, 0, 3));

    Matrix4F vp = p_camera->get_view_projection();

    float time = 0.0f;
    while (m_running) {
        poll_window_events();

        monkey_transform = monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

        render_pipeline.render_frame(*p_camera, renderables);

        time += 1.0f / 300.0f;
    }
}

void Application::poll_window_events() {
    XEvent event;

    while(m_window.poll_event(event)) {
        // resize from the server event
        if (event.type == Expose)  {
            uint32_t width = m_window.m_width;
            uint32_t height = m_window.m_height;

            p_camera->width = width;
            p_camera->height = height;

            Matrix4F projection;
            float aspect_ratio = (float)width / (float)height;
            projection.init_perspective(deg_to_half_rad(90.0f) / aspect_ratio, aspect_ratio, 0.1f, 1000.0f);
            p_camera->set_projection(projection);

            InputEvent e {
                .body = {
                    .value = (height << 16) | width,
                },
                .event_type = EventType::Window,
            };

            emit_event(e, EventType::Window);
        }
    }
}

GWindow* Application::get_window() {
    return &m_window;
}

void Application::send_window_event(InputEvent event) {
    emit_event(event, EventType::Window);
}
