#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/Camera.h"
#include "../graphics/RenderPipeline.h"

Application* Application::sp_instance = nullptr;

Application::Application() {

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

    Matrix4F projection;
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);

    Transform monkey_transform = Transform(V4F(0, 0, 2));
    Camera camera{projection};

    Matrix4F vp = camera.get_view_projection();

    float time = 0.0f;
    while (m_running) {
        poll_window_events();

        monkey_transform = monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

        render_pipeline.render_frame(camera, renderables);

        time += 1.0f / 300.0f;
    }
}

void Application::poll_window_events() {
    XEvent event;

    while(m_window.poll_event(event)) {
        if (event.type == Expose)  {

        }
    }
}

GWindow* Application::get_window() {
    return &m_window;
}
