#include "Camera.h"

Camera::Camera(const Matrix4F &projection) {
    m_projection = projection;
}

Matrix4F Camera::get_view_projection() {
    m_transform = m_transform.set_pos(V4F(0, 10, -4));
    //m_transform = m_transform.rotate(Quaternion(V4F(0, 0, 0), 3));
    //m_transform = m_transform.look_at(V4F(0, -1, 1), V4F(0, 1, 1));

    Matrix4F camera_rot = m_transform.rot.conjugate().to_rotation_matrix();
    V4F camera_pos = m_transform.pos * -1;

    Matrix4F camera_trans;
    camera_trans.init_translation(camera_pos.x, camera_pos.y, camera_pos.z);

    //camera_rot.init_rotation(V4F(0,-1,1), V4F(0, 1, 1), V4F(1, 0, 0));
    camera_rot.init_rotation(V4F(0,-1,1), V4F(0, 1, 1));


    return  camera_trans * camera_rot * m_projection;
}

