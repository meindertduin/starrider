#include <list>

#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer), m_rasterizer(renderer) {

}

void RenderPipeline::render_viewport(const Camera &camera, std::vector<Renderable> renderables) {
    m_rasterizer.set_viewport(camera.width, camera.height);

    p_renderer->clear_screen();
    m_rasterizer.clear_depth_buffer();

    auto vp = camera.get_transformation_matrix();
    auto mat_proj = camera.get_projection_matrix();

    auto v_forward = camera.m_transform.rot.get_back();

    for (auto renderable : renderables) {
        Matrix4x4 transform = renderable.transform->get_matrix_transformation();
        for (auto triangle : renderable.mesh->triangles) {
            Triangle translated_tri;

            translated_tri.p[0] = triangle.p[0].transform(transform);
            translated_tri.p[1] = triangle.p[1].transform(transform);
            translated_tri.p[2] = triangle.p[2].transform(transform);

            auto camera_ray =  camera.m_transform.pos - translated_tri.p[0].pos;

            if (translated_tri.p[0].normal.dot(camera_ray) > 0) {
                Triangle proj_tri;
                proj_tri.p[0] = translated_tri.p[0].transform(vp, transform);
                proj_tri.p[1] = translated_tri.p[1].transform(vp, transform);
                proj_tri.p[2] = translated_tri.p[2].transform(vp, transform);

                proj_tri.p[0] = proj_tri.p[0].transform(mat_proj);
                proj_tri.p[1] = proj_tri.p[1].transform(mat_proj);
                proj_tri.p[2] = proj_tri.p[2].transform(mat_proj);

                m_rasterizer.draw_triangle(proj_tri, *renderable.mesh->texture);
            }
        }
    }
}
