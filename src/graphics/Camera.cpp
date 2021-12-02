#include "Camera.h"

Camera::Camera(const Matrix4F &projection) {
    m_projection = projection;
}

Matrix4F Camera::get_view_projection() {
    m_transform = m_transform.set_pos(V4F(0, 0, -20));
    Matrix4F camera_rot = m_transform.rot.conjugate().to_rotation_matrix();
    V4F camera_pos = m_transform.pos * -1;

    Matrix4F camera_trans;
    camera_trans.init_translation(camera_pos.x, camera_pos.y, camera_pos.z);

    return camera_rot * camera_trans * m_projection;
}

