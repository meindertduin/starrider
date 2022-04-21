#include <stdio.h>

#include "Application.h"
#include "../graphics/Core.h"
#include "../graphics/Renderer.h"
#include "../graphics/Texture.h"
#include "../graphics/Font.h"
#include "../graphics/RenderPipeline.h"
#include "../graphics/ObjectRepository.h"

#include "../math/Core.h"

#include "KeyMap.h"
#include "Time.h"

std::shared_ptr<Application> Application::sp_instance = std::shared_ptr<Application>(nullptr);

Application::Application() {
    m_fps = 60;
}

Application::~Application() {
    delete[] m_rc.inv_z_buffer;
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

    Graphics::Renderer renderer;
    Graphics::RenderPipeline render_pipeline {&renderer};

    auto win_width = m_window.get_width();
    auto win_height = m_window.get_height();

    m_rc.attributes = Graphics::RCAttributeMipMapped
        | Graphics::RCAttributeINVZBuffer
        | Graphics::RCAttributeTextureHybrid
        | Graphics::RCAttributeZSort;

    m_rc.mip_z_dist = 200;
    m_rc.perfect_dist = 20;
    m_rc.piecewise_dist = 40;

    m_rc.inv_z_buffer = new float[win_width * win_height];

    m_rc.min_clip_x = 0;
    m_rc.max_clip_x = win_width;

    m_rc.min_clip_y = 0;
    m_rc.max_clip_y = win_height;

    Graphics::reset_materials();
    Graphics::reset_lights();

    Graphics::create_base_amb_light(0, Graphics::RGBA { 25, 25, 25, 255 });
    Graphics::create_base_dir_light(1, Graphics::RGBA { 255, 255, 255, 255 }, V4D(1, 0, 0).normalized());

    //create_base_point_light(2, RGBA { 225, 225, 225, 255 }, V4D(0, 1, 0), 0, 0, 0.2f);

    // Setup camera
    p_camera = std::make_unique<Graphics::Camera>();
    p_camera->set_viewport(m_window.get_width(), m_window.get_height());

    m_cursor.initialize(&m_window);

    std::vector<Graphics::RenderObject> objects;
    Graphics::ObjectRepository object_repository;

    auto object = object_repository.create_render_object("assets/test.mde");
    auto plateau = object_repository.create_render_object("assets/plateau.mde");

    object.transform = Graphics::Transform(V4D(0, 0, 3));
    plateau.transform = Graphics::Transform(V4D(0, -5, 0));

    objects.push_back(object);
    objects.push_back(plateau);

    Graphics::TTFFont ttf_font("assets/alagard.ttf", 24);
    int dt = 0;

    while (m_running) {
        m_cursor.reset_pos_middle();

        auto cycle_start = get_program_ticks_ms();

        poll_window_events();

        render_pipeline.render_objects(*p_camera, objects, m_rc);

        string time_text = std::to_string(dt) + "MS";
        renderer.render_text(time_text, ttf_font, {20, 52});

        renderer.render_framebuffer();
        // objects[0].transform.rotate(Quat_Type(V4D(0, 1, 0), Math::deg_to_rad(1)));


        // objects[0].transform.rotate(Quat_Type(Math::mat_4x4_rotation_y(dt / 1000.0f)));

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
                if (m_rc.max_clip_x != event.body.expose_event.width || m_rc.max_clip_y != event.body.expose_event.height) {
                    delete[] m_rc.inv_z_buffer;

                    m_rc.max_clip_x = event.body.expose_event.width;
                    m_rc.max_clip_y = event.body.expose_event.height;
                    m_rc.inv_z_buffer = new float[event.body.expose_event.width * event.body.expose_event.height];
                }

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
