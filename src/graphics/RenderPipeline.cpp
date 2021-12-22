#include <list>

#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer), m_rasterizer(renderer) {

}

void RenderPipeline::render_frame(const Camera &camera, std::vector<Renderable> renderables) {
    m_rasterizer.set_viewport(camera.width, camera.height);

    p_renderer->clear_screen();
    m_rasterizer.clear_depth_buffer();

    auto vp = camera.get_view_projection();

    for (auto renderable : renderables) {
        Matrix4F transform = renderable.transform->get_matrix_transformation();
        for (auto triangle : renderable.mesh->triangles) {
            int clipped_triangles = 0;
            Triangle clipped[2];

            // TODO: update these values with the current position of the camera
            V4F near_plane =  V4F(0.0f, 0.0f, 0.1f);
            V4F near_normal_plane = V4F(0.0f, 0.0f, 1.0f);

            Triangle translated_tri;
            translated_tri.p[0] = triangle.p[0].transform(transform);
            translated_tri.p[1] = triangle.p[1].transform(transform);
            translated_tri.p[2] = triangle.p[2].transform(transform);

            Triangle proj_tri;

            proj_tri.p[0] = translated_tri.p[0].transform(vp, transform);
            proj_tri.p[1] = translated_tri.p[1].transform(vp, transform);
            proj_tri.p[2] = translated_tri.p[2].transform(vp, transform);

            m_rasterizer.draw_triangle(proj_tri, *renderable.mesh->texture);

            // clipped_triangles = triangle_clip_against_plane(near_plane, near_normal_plane, translated_tri, clipped[0], clipped[1]);

            //for (int n = 0; n < clipped_triangles; n++) {

                //Triangle new_clipped[2];
                //std::list<Triangle> list_triangles_front;
                //list_triangles_front.push_back(proj_tri);
                //int n_new_triangles = 1;

                //for (int p = 0; p < 4; p++) {
                //    int nTrisToAdd = 0;
                //    while (n_new_triangles > 0) {
                //        // Take triangle from front of queue
                //        Triangle test = list_triangles_front.front();
                //        list_triangles_front.pop_front();
                //        n_new_triangles--;

                //        switch (p) {
                //            case 0:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, 0.0f, 0.0f), V4F( 0.0f, 1.0f, 0.0f ), test, clipped[0], clipped[1]); break;
                //            case 1:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, camera.height - 1, 0.0f), V4F(0.0f, -1.0f, 0.0f), test, clipped[0], clipped[1]); break;
                //            case 2:	nTrisToAdd = triangle_clip_against_plane(V4F(0.0f, 0.0f, 0.0f), V4F(1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
                //            case 3:	nTrisToAdd = triangle_clip_against_plane(V4F(camera.width - 1, 0.0f, 0.0f), V4F(-1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
                //        }

                //        for (int w = 0; w < nTrisToAdd; w++)
                //            list_triangles_front.push_back(clipped[w]);
                //    }

                //    n_new_triangles = list_triangles_front.size();
                //}

                //for (auto &t : list_triangles_front) {
                //    m_rasterizer.draw_triangle(t, *renderable.mesh->texture);
                //}
            //}
        }
    }

    p_renderer->render();
}
