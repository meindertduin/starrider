#include <stdio.h>
#include <X11/Xlib.h>

#include "graphics/Renderer.h"

int main() {
    GWindow window;
    if (!window.initialize()) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return 1;
    }

    Renderer renderer {&window};

    Triangle tri;

    tri.p[0] = Vertex(-1, -1, 0);
    tri.p[1] = Vertex(0, 1, 0);
    tri.p[2] = Vertex(1, -1, 0);

    Matrix4F identity, projection, translation, rotation_y;
    identity.init_identity();
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);

    XEvent event;

    float time = 0.0f;

    for (;;) {
        while(window.poll_event(event)) {
            if (event.type == Expose)  {
            }
        }

        renderer.clear_screen();

        Triangle translated_tri;
        translation.init_translation(0.0001f, 0.0f, 3.0f);

        Triangle proj_tri;
        Matrix4F rotation_y;

        rotation_y.init_rotation_y(time);
        Matrix4F transform = identity * rotation_y * translation * projection;

        Triangle transformed_triangle;

        proj_tri.p[0] = tri.p[0].transform(transform);
        proj_tri.p[1] = tri.p[1].transform(transform);
        proj_tri.p[2] = tri.p[2].transform(transform);

        Matrix4F screen_space;

        screen_space.init_screen_space_transform(400.0f, 400.0f);

        proj_tri.p[0] = proj_tri.p[0].transform(screen_space).perspective_divide();
        proj_tri.p[1] = proj_tri.p[1].transform(screen_space).perspective_divide();
        proj_tri.p[2] = proj_tri.p[2].transform(screen_space).perspective_divide();

        proj_tri.p[0].color = Color(1, 0, 0);
        proj_tri.p[1].color = Color(0, 1, 0);
        proj_tri.p[2].color = Color(0, 0, 1);

        renderer.draw_triangle(proj_tri);
        renderer.render();

        time += 1.0f / 300.0f;
    }

    return 0;
}

