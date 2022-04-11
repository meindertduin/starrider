#include "Camera.h"
#include "../core/Application.h"
#include "../core/KeyMap.h"

#include "../math/Core.h"
#include "../math/Matrix.h"

namespace Graphics {
Camera::Camera() {
    m_zfar = 1000.0f;
    m_znear = 1.0f;
    m_fov = 90.0f;
    aspect_ratio = 1.0f;

    float viewplane_width = 2.0f;
    float viewplane_height = 2.0f;

    tan_fov_div2 = std::tan(Math::deg_to_rad(m_fov / 2.0f));

    view_dist_h = 0.5f * viewplane_width * tan_fov_div2;
    view_dist_v = 0.5f * viewplane_height * tan_fov_div2; // TODO figure out of aspect ratio needs to be applied here

    auto app = Application::get_instance();
    app->listen(this, WindowEventType::KeyDown | WindowEventType::MouseMotion);
}

Camera::~Camera() {
    auto app = Application::get_instance();
    app->unlisten(this, WindowEventType::KeyDown);
}

void Camera::on_event(const WindowEvent &event) {
    switch(event.event_type) {
        case WindowEventType::KeyDown:
            switch(event.body.keyboard_event.keysym) {
                case K_UP:
                    m_transform.move(m_transform.rot.get_forward(), 0.2f);
                    break;
                case K_DOWN:
                    m_transform.move(m_transform.rot.get_back(), 0.2f);
                    break;
                case K_LEFT:
                    m_transform.move(m_transform.rot.get_left(), 0.2f);
                    break;
                case K_RIGHT:
                    m_transform.move(m_transform.rot.get_right(), 0.2f);
                    break;
                case K_q:
                    m_transform = m_transform.rotate(Quat_Type(V4D(0, 1, 0), Math::deg_to_rad(-1.0f)));
                    break;
                case K_e:
                    m_transform = m_transform.rotate(Quat_Type(V4D(0, 1, 0), Math::deg_to_rad(1.0f)));
                default:
                    break;
            }
            break;
        case WindowEventType::MouseMotion:
            {
                float d_xmovment = (float)event.body.mouse_event.d_xpos;
                float d_ymovment = (float)event.body.mouse_event.d_ypos;

                m_transform.rotate(Quat_Type(V4D(0, 1, 0), Math::deg_to_rad(1.0f * (d_xmovment / 100.f))));
                auto right = m_transform.rot.get_right();
                m_transform.rotate(Quat_Type(V4D(right.x, right.y, right.z, right.w), Math::deg_to_rad(1.0f * (d_ymovment / 100.f))));
            }
            break;
        default:
            break;
    }
}

void Camera::set_viewport(int width, int height) {
    this->width = width;
    this->height = height;

    aspect_ratio = (float) width / (float) height;
}

Matrix4x4 Camera::get_view_projection() const {
    V4D camera_pos = m_transform.pos * -1;
    auto camera_trans = Math::mat_4x4_translation(camera_pos.x, camera_pos.y, camera_pos.z);

    return  camera_trans * m_transform.rot.conjugated().to_rotation_matrix();
}

}
