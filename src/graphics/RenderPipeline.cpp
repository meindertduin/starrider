#include <list>

#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer), m_rasterizer(renderer) {

}

void RenderPipeline::render_objects(const Camera &camera, std::vector<RenderObject> renderables) {
    m_rasterizer.set_viewport(camera.width, camera.height);

    p_renderer->clear_screen();
    m_rasterizer.clear_depth_buffer();

    auto vp = camera.get_view_projection();

    auto v_forward = camera.m_transform.rot.get_back();

    for (auto renderable : renderables) {
        Matrix4x4 transform = renderable.transform.get_matrix_transformation();

        transform_world_pos(renderable);

        for (int i = 0; i < renderable.poly_count; i++) {
            auto current_poly = renderable.polygons[i];

            renderable.transformed_points[current_poly.vert[0]] =
                transform.transform(renderable.local_points[current_poly.vert[0]]);

            renderable.transformed_points[current_poly.vert[1]] =
                transform.transform(renderable.local_points[current_poly.vert[1]]);

            renderable.transformed_points[current_poly.vert[2]] =
                transform.transform(renderable.local_points[current_poly.vert[2]]);
        }

        for (int i = 0; i < renderable.poly_count; i++) {
            auto current_poly = renderable.polygons[i];

            auto line1 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[1]];

            auto line2 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[2]];

            auto camera_ray =  camera.m_transform.pos - renderable.transformed_points[current_poly.vert[0]];
            current_poly.normal = line1.cross(line2).normalized();

            if (current_poly.normal.dot(camera_ray) > 0) {
                Triangle proj_tri;

                proj_tri.p[0] = Vertex {
                    vp.transform(renderable.transformed_points[current_poly.vert[0]]),
                    renderable.text_coords[current_poly.text[0]],
                    current_poly.normal
                };

                proj_tri.p[1] = Vertex {
                    vp.transform(renderable.transformed_points[current_poly.vert[1]]),
                    renderable.text_coords[current_poly.text[1]],
                    current_poly.normal
                };

                proj_tri.p[2] = Vertex {
                    vp.transform(renderable.transformed_points[current_poly.vert[2]]),
                    renderable.text_coords[current_poly.text[2]],
                    current_poly.normal
                };

                m_rasterizer.draw_triangle(proj_tri, *renderable.texture);
            }
        }
    }
}

void RenderPipeline::transform_world_pos(RenderObject &object, CoordSelect coord_select) {
    auto world_pos = object.transform.pos;
    if (coord_select == CoordSelect::Local_To_Trans) {
        for (int i = 0; i < object.vertex_count; ++i)
            object.transformed_points[i] = object.local_points[i] + world_pos;
    } else { // TransOnly
        for (int i = 0; i < object.vertex_count; ++i)
            object.transformed_points[i] = object.transformed_points[i] + world_pos;
    }
}
