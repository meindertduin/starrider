#pragma once

#include "Core.h"

class Camera {
public:
    Camera();
    void set_viewport(int width, int height);
    Matrix4F get_view_projection() const;
    int width, height;
private:
    float m_zfar;
    float m_znear;
    float m_fov;

    Matrix4F m_projection;
    Transform m_transform;
    V4F y_axis = V4F(0, 1, 0);
};
