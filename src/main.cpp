#include <stdio.h>
#include <X11/Xlib.h>

#include "graphics/Core.h"
#include "graphics/Renderer.h"
#include "graphics/Rasterizer.h"

int main() {
    GWindow window;
    if (!window.initialize()) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return 1;
    }

    Bitmap texture("test_texture.bmp");
    Renderer renderer {&window};
    Rasterizer rasterizer {&renderer};
    Mesh mesh;
    mesh.load_from_obj_file("teapot.obj");

    texture.width = 16;
    texture.height = 16;

    Matrix4F identity, projection, translation, rotation_y, screen_space;
    identity.init_identity();
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);
    screen_space.init_screen_space_transform(400.0f, 400.0f);
    translation.init_translation(0, 0.0f, 8.0f);

    XEvent event;

    float time = 0.0f;

    for (;;) {
        while(window.poll_event(event)) {
            if (event.type == Expose)  {
            }
        }

        renderer.clear_screen();

        Triangle translated_tri;
        rotation_y.init_rotation(0, time, time);
        Matrix4F transform = identity * rotation_y * translation * projection;

        for (auto triangle : mesh.triangles) {
            triangle.p[0].text_coords = V4F(0, 0, 0);
            triangle.p[1].text_coords = V4F(0.5, 1, 1);
            triangle.p[2].text_coords = V4F(1, 0, 0);

            Triangle proj_tri;

            proj_tri.p[0] = triangle.p[0].transform(transform);
            proj_tri.p[1] = triangle.p[1].transform(transform);
            proj_tri.p[2] = triangle.p[2].transform(transform);

            proj_tri.p[0] = proj_tri.p[0].transform(screen_space).perspective_divide();
            proj_tri.p[1] = proj_tri.p[1].transform(screen_space).perspective_divide();
            proj_tri.p[2] = proj_tri.p[2].transform(screen_space).perspective_divide();

            rasterizer.draw_triangle(proj_tri, texture);
        }

        renderer.render();
        time += 1.0f / 300.0f;
    }

    return 0;
}

