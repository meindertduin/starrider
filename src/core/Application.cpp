#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/RenderPipeline.h"
#include "../graphics/Texture.h"
#include "../graphics//Font.h"

#include "../math/Core.h"

#include "KeyMap.h"
#include "Time.h"

std::shared_ptr<Application> Application::sp_instance = std::shared_ptr<Application>(nullptr);

Application::Application() {
    m_fps = 60;
}

std::shared_ptr<Application> Application::get_instance() {
    if (sp_instance == nullptr) {
        sp_instance = std::shared_ptr<Application>(new Application());
    }

    return sp_instance;
}

bool Application::initialize(const AppSettings &settings) {
    WindowSettings win_settings = {
        .width = settings.win_width,
        .height = settings.win_height,
        .win_display_mode = WDisplayMode::Floating,
    };

    bool success = m_window.initialize(win_settings);
    if (!success) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return false;
    }

    return true;
}

void Application::run() {
    m_running = true;

    p_camera = std::make_unique<Camera>();
    p_camera->set_viewport(m_window.get_width(), m_window.get_height());

    m_cursor.initialize(&m_window);

    Texture brick_texture;
    brick_texture.load_from_bmp("assets/test_texture.bmp");
    Texture grass_texture;
    grass_texture.load_from_bmp("assets/grass.bmp");

    Renderer renderer;
    RenderPipeline render_pipeline {&renderer};

    Mesh mesh;
    mesh.load_from_obj_file("assets/monkey.obj");
    mesh.texture = &brick_texture;

    Mesh terrain;
    terrain.load_from_obj_file("assets/plateau.obj");
    terrain.texture = &brick_texture;

    Transform monkey_transform = Transform(V4D(0, 0, 3));
    Transform terrain_transform = Transform(V4D(0, -4, 0));

    TTFFont ttf_font("assets/alagard.ttf", 24);
    int dt = 0;

    while (m_running) {
        m_cursor.reset_pos_middle();

        auto cycle_start = get_program_ticks_ms();

        poll_window_events();

        monkey_transform.rotate(Quat_Type(V4D(0, 1, 0), Math::deg_to_rad(0.5f)));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

        renderables.push_back({
            .transform = &terrain_transform,
            .mesh = &terrain,
        });

       render_pipeline.render_viewport(*p_camera, renderables);

       string time_text = std::to_string(dt) + "MS";
       renderer.render_text(time_text, ttf_font, {20, 52});

       renderer.render_framebuffer();

       dt = static_cast<int>(get_program_ticks_ms() - cycle_start);

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
