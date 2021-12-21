#include "Camera.h"
#include "../math/Radians.h"
#include "../core/Application.h"
#include "../core/KeyMap.h"

Camera::Camera() {
    m_zfar = 1000.0f;
    m_znear = 0.1f;
    m_fov = 90.0f;

    m_projection.init_perspective(deg_to_half_rad(m_fov), 1.0f, m_znear, m_zfar);

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
                    m_transform.pos.z += 0.3f;
                    break;
                case K_DOWN:
                    m_transform.pos.z -= 0.3f;
                    break;
                case K_LEFT:
                    m_transform.pos.x -= 0.3f;
                    break;
                case K_RIGHT:
                    m_transform.pos.x += 0.3f;
                    break;
                case K_q:
                    m_transform = m_transform.rotate(Quaternion(V4F(0, 1, 0), -1.0f));
                    break;
                case K_e:
                    m_transform = m_transform.rotate(Quaternion(V4F(0, 1, 0), 1.0f));
                default:
                    break;
            }
            break;
        case WindowEventType::MouseMotion:
            {
                float d_xmovment = (float)event.body.mouse_event.d_xpos;
                float d_ymovment = (float)event.body.mouse_event.d_ypos;

                m_transform.rotate(Quaternion(V4F(0, 1, 0), 1.0f * (d_xmovment / 100.f)));
                m_transform.rotate(Quaternion(V4F(1, 0, 0), 1.0f * (d_ymovment / 100.f)));
            }
            break;
        default:
            break;
    }
}

void Camera::set_viewport(int width, int height) {
    this->width = width;
    this->height = height;

    float aspect_ratio = (float) width / (float) height;
    m_projection.init_perspective(deg_to_half_rad(m_fov) / aspect_ratio, aspect_ratio, m_znear, m_zfar);
}

Matrix4F Camera::get_view_projection() const {
    V4F camera_pos = m_transform.pos * -1;

    Matrix4F camera_trans;
    camera_trans.init_translation(camera_pos.x, camera_pos.y, camera_pos.z);

    return  camera_trans * m_transform.rot.conjugate().to_rotation_matrix() * m_projection;
}

