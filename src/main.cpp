#include <stdio.h>
#include <X11/Xlib.h>
#include <list>

#include "graphics/Core.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include "graphics/RenderPipeline.h"

int main() {
    GWindow window {800, 800};
    if (!window.initialize()) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return 1;
    }

    Bitmap texture("test_texture.bmp");
    Renderer renderer {&window};

    Rasterizer rasterizer {&renderer, window.m_width, window.m_height};

    RenderPipeline render_pipeline {&renderer};

    // TODO: Values that may belong to a scene?
    Mesh mesh;
    mesh.load_from_obj_file("monkey0.obj");
    mesh.texture = &texture;

    Matrix4F projection;
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);

    Transform monkey_transform = Transform(V4F(0, 0, 2));
    Camera camera{projection};

    float time = 0.0f;
    XEvent event;

    for (;;) {
        Matrix4F vp = camera.get_view_projection();

        while(window.poll_event(event)) {
            if (event.type == Expose)  {
            }
        }

        monkey_transform = monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5));

        std::vector<Renderable> renderables;
        renderables.push_back({
            .transform = &monkey_transform,
            .mesh = &mesh,
        });

        render_pipeline.render_frame(camera, renderables);

        time += 1.0f / 300.0f;
    }

    return 0;
}

