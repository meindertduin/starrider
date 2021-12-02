#pragma once

#include "Core.h"

class Camera {
public:
    Camera(const Matrix4F &projection);
    Matrix4F* get_view_projection();
private:
    Matrix4F m_projection;
    V4F y_axis = V4F(0, 1, 0);
};
