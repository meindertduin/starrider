#include <list>

#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer), m_rasterizer(renderer) {

}

void RenderPipeline::render_viewport(const Camera &camera, std::vector<Renderable> renderables) {
    m_rasterizer.set_viewport(camera.width, camera.height);

    p_renderer->clear_screen();
    m_rasterizer.clear_depth_buffer();

    auto vp = camera.get_view_projection();

    auto v_forward = camera.m_transform.rot.get_back();

    for (auto renderable : renderables) {
        Matrix4x4 transform = renderable.transform->get_matrix_transformation();
        for (auto triangle : renderable.mesh->triangles) {
            Triangle translated_tri;

            translated_tri.p[0] = triangle.p[0].transform(transform);
            translated_tri.p[1] = triangle.p[1].transform(transform);
            translated_tri.p[2] = triangle.p[2].transform(transform);

            auto line1 = translated_tri.p[0].pos - translated_tri.p[1].pos;
            auto line2 = translated_tri.p[0].pos - translated_tri.p[2].pos;

            auto camera_ray =  camera.m_transform.pos - translated_tri.p[0].pos;
            auto surface_normal = line1.cross(line2).normalized();

            if (surface_normal.dot(camera_ray) > 0) {
                Triangle proj_tri;
                proj_tri.p[0] = translated_tri.p[0].transform(vp, transform);
                proj_tri.p[1] = translated_tri.p[1].transform(vp, transform);
                proj_tri.p[2] = translated_tri.p[2].transform(vp, transform);

                m_rasterizer.draw_triangle(proj_tri, *renderable.mesh->texture);
            }
        }
    }
}
