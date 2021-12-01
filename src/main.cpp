#include <stdio.h>
#include <X11/Xlib.h>
#include <list>

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
    Rasterizer rasterizer {&renderer, 800, 800};
    Mesh mesh;
    mesh.load_from_obj_file("teapot.obj");

    texture.width = 16;
    texture.height = 16;

    Matrix4F identity, projection, translation, rotation_y, screen_space;
    identity.init_identity();
    float rad = 1.0 / std::tan(90.0f * 0.5f / 180.0f * 3.14159f);
    projection.init_perspective(rad, 1.0f, 0.1f, 1000.0f);
    screen_space.init_screen_space_transform(400.0f, 400.0f);
    translation.init_translation(0.0f, 3, 4.0f);

    XEvent event;

    float time = 0.0f;

    for (;;) {
        while(window.poll_event(event)) {
            if (event.type == Expose)  {
            }
        }

        renderer.clear_screen();
        rasterizer.clear_depth_buffer();

        Triangle translated_tri;
        rotation_y.init_rotation(0, time, time);
        Matrix4F transform = identity * rotation_y * translation;

        for (auto triangle : mesh.triangles) {
            triangle.p[0].text_coords = V4F(0, 0, 0);
            triangle.p[1].text_coords = V4F(0.5, 1, 0);
            triangle.p[2].text_coords = V4F(1, 0, 0);

            int clipped_triangles = 0;
            Triangle clipped[2];
            V4F near_plane =  V4F(0.0f, 0.0f, 0.1f);
            V4F near_normal_plane = V4F(0.0f, 0.0f, 1.0f);

            Triangle translated_tri;
            translated_tri.p[0] = triangle.p[0].transform(transform);
            translated_tri.p[1] = triangle.p[1].transform(transform);
            translated_tri.p[2] = triangle.p[2].transform(transform);

            clipped_triangles = triangle_clip_against_plane(near_plane, near_normal_plane, translated_tri, clipped[0], clipped[1]);
            for (int n = 0; n < clipped_triangles; n++) {
                Triangle proj_tri;

                proj_tri.p[0] = clipped[n].p[0].transform(projection);
                proj_tri.p[1] = clipped[n].p[1].transform(projection);
                proj_tri.p[2] = clipped[n].p[2].transform(projection);

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

