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
        Matrix4x4 mat_rot = renderable.transform.get_rotation_matrix();

        for (int i = 0; i < renderable.poly_count; i++) {
            auto current_poly = renderable.polygons[i];

            renderable.transformed_points[current_poly.vert[0]] =
                mat_rot.transform(renderable.local_points[current_poly.vert[0]]);

            renderable.transformed_points[current_poly.vert[1]] =
                mat_rot.transform(renderable.local_points[current_poly.vert[1]]);

            renderable.transformed_points[current_poly.vert[2]] =
                mat_rot.transform(renderable.local_points[current_poly.vert[2]]);
        }

        translation_scale_transform(renderable, CoordSelect::Trans_Only);

        for (int i = 0; i < renderable.poly_count; i++) {
            auto current_poly = renderable.polygons[i];

            if (renderable.state & PolyAttribute::TwoSided) {
                auto line1 = renderable.transformed_points[current_poly.vert[0]]
                    - renderable.transformed_points[current_poly.vert[1]];

                auto line2 = renderable.transformed_points[current_poly.vert[0]]
                    - renderable.transformed_points[current_poly.vert[2]];

                auto camera_ray =  camera.m_transform.pos - renderable.transformed_points[current_poly.vert[0]];
                current_poly.normal = line1.cross(line2).normalized();

                if (current_poly.normal.dot(camera_ray) >= 0) {
                    auto proj_tri = get_proj_tri(renderable, current_poly, vp);
                    m_rasterizer.draw_triangle(proj_tri, *renderable.texture);
                }
            } else {

                auto line1 = renderable.transformed_points[current_poly.vert[0]]
                    - renderable.transformed_points[current_poly.vert[1]];

                auto line2 = renderable.transformed_points[current_poly.vert[0]]
                    - renderable.transformed_points[current_poly.vert[2]];

                current_poly.normal = line1.cross(line2).normalized();
                auto proj_tri = get_proj_tri(renderable, current_poly, vp);
                m_rasterizer.draw_triangle(proj_tri, *renderable.texture);
            }
        }
    }
}

void RenderPipeline::translation_scale_transform(RenderObject &object, CoordSelect coord_select) {
    auto world_pos = object.transform.pos;
    if (coord_select == CoordSelect::Local_To_Trans) {
        for (int i = 0; i < object.vertex_count; ++i) {
            object.transformed_points[i] = object.local_points[i] + world_pos;
            object.transformed_points[i].x *= object.transform.scale.x;
            object.transformed_points[i].y *= object.transform.scale.y;
            object.transformed_points[i].z *= object.transform.scale.z;
        }
    } else { // TransOnly
        for (int i = 0; i < object.vertex_count; ++i) {
            object.transformed_points[i] = object.transformed_points[i] + world_pos;
            object.transformed_points[i].x *= object.transform.scale.x;
            object.transformed_points[i].y *= object.transform.scale.y;
            object.transformed_points[i].z *= object.transform.scale.z;
        }
    }
}
