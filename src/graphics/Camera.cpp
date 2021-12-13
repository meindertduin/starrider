#include "Camera.h"
#include "../math/Radians.h"

Camera::Camera() {
    m_zfar = 1000.0f;
    m_znear = 0.1f;
    m_fov = 90.0f;

    m_projection.init_perspective(deg_to_half_rad(m_fov), 1.0f, m_znear, m_zfar);
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
