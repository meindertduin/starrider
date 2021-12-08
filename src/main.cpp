#include <stdio.h>
#include <X11/Xlib.h>
#include <list>

#include "graphics/Core.h"
#include "graphics/Renderer.h"
#include "graphics/Rasterizer.h"
#include "graphics/Camera.h"

int main() {
    GWindow window;
    if (!window.initialize()) {
        printf("Graphics Error: Something went wrong while creating the main window\n");
        return 1;
    }

    Bitmap texture("test_texture.bmp");
    Renderer renderer {&window};
    Rasterizer rasterizer {&renderer, 800, 800};
    Mesh mesh;
    mesh.load_from_obj_file("monkey0.obj");

    texture.width = 16;
    texture.height = 16;

    Matrix4F projection, screen_space;
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);
    screen_space.init_screen_space_transform(400.0f, 400.0f);

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

        renderer.clear_screen();
        rasterizer.clear_depth_buffer();

        Triangle translated_tri;
        monkey_transform = monkey_transform.rotate(Quaternion(V4F(0, 1, 0), 0.5));
        Matrix4F transform = monkey_transform.get_matrix_transformation();

        for (auto triangle : mesh.triangles) {
            int clipped_triangles = 0;
            Triangle clipped[2];

            // TODO: update these values with the current position of the camera
            V4F near_plane =  V4F(0.0f, 0.0f, 0.1f);
            V4F near_normal_plane = V4F(0.0f, 0.0f, 1.0f);

            Triangle translated_tri;
            translated_tri.p[0] = triangle.p[0].transform(transform);
            translated_tri.p[1] = triangle.p[1].transform(transform);
            translated_tri.p[2] = triangle.p[2].transform(transform);

            clipped_triangles = triangle_clip_against_plane(near_plane, near_normal_plane, translated_tri, clipped[0], clipped[1]);
            for (int n = 0; n < clipped_triangles; n++) {
                Triangle proj_tri;

                proj_tri.p[0] = clipped[n].p[0].transform(vp);
                proj_tri.p[1] = clipped[n].p[1].transform(vp);
                proj_tri.p[2] = clipped[n].p[2].transform(vp);

                proj_tri.p[0].normal_transform(transform);
                proj_tri.p[1].normal_transform(transform);
                proj_tri.p[2].normal_transform(transform);

                proj_tri.p[0] = proj_tri.p[0].transform(screen_space).perspective_divide();
                proj_tri.p[1] = proj_tri.p[1].transform(screen_space).perspective_divide();
                proj_tri.p[2] = proj_tri.p[2].transform(screen_space).perspective_divide();


                Triangle new_clipped[2];
                std::list<Triangle> list_triangles_front;
                list_triangles_front.push_back(proj_tri);
                int n_new_triangles = 1;

                for (int p = 0; p < 4; p++) {
                    int nTrisToAdd = 0;
                    while (n_new_triangles > 0) {
                        // Take triangle from front of queue
                        Triangle test = list_triangles_front.front();
                        list_triangles_front.pop_front();
                        n_new_triangles--;

                        switch (p) {
                            case 0:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, 0.0f, 0.0f), V4F( 0.0f, 1.0f, 0.0f ), test, clipped[0], clipped[1]); break;
                            case 1:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, 800.0f - 1, 0.0f), V4F(0.0f, -1.0f, 0.0f), test, clipped[0], clipped[1]); break;
                            case 2:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, 0.0f, 0.0f), V4F(1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
                            case 3:	nTrisToAdd = triangle_clip_against_plane(V4F(800.0f - 1, 0.0f, 0.0f), V4F(-1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
                        }

                        for (int w = 0; w < nTrisToAdd; w++)
                            list_triangles_front.push_back(clipped[w]);
                    }

                    n_new_triangles = list_triangles_front.size();
                }

                for (auto &t : list_triangles_front) {
                    rasterizer.draw_triangle(t, texture);
                }
            }
        }

        renderer.render();
        time += 1.0f / 300.0f;
    }

    return 0;
}

