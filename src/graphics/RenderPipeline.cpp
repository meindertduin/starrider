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
        }
    }

    p_renderer->render();
}
