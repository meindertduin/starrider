#pragma once

#include "Core.h"
#include "../core/Events.h"

class Camera : public EventObserver<WindowEvent> {
public:
    Camera();
    ~Camera();
    void set_viewport(int width, int height);
    Matrix4F get_view_projection() const;
    int width;
    int height;

    void on_event(const WindowEvent &event) override;
    Transform m_transform;
private:
    float m_zfar;
    float m_znear;
    float m_fov;

    Matrix4F m_projection;
    V4F y_axis = V4F(0, 1, 0);
};
