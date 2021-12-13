#pragma once

#include "Core.h"

class Camera {
public:
    Camera(const Matrix4F &projection);
    void set_projection(const Matrix4F &projection);
    Matrix4F get_view_projection() const;
    int width, height;
private:
    Matrix4F m_projection;
    Transform m_transform;
    V4F y_axis = V4F(0, 1, 0);
};
