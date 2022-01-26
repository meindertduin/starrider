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

        for (int i = 0; i < renderable.vertex_count; i++) {
            renderable.transformed_points[i] = mat_rot.transform(renderable.local_points[i]);

            renderable.transformed_points[i] = renderable.transformed_points[i] + renderable.transform.pos;
            renderable.transformed_points[i].x *= renderable.transform.scale.x;
            renderable.transformed_points[i].y *= renderable.transform.scale.y;
            renderable.transformed_points[i].z *= renderable.transform.scale.z;
        }

        for (int i = 0; i < renderable.poly_count; i++) {
            auto current_poly = renderable.polygons[i];

            auto line1 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[1]];

            auto line2 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[2]];

            auto camera_ray =  camera.m_transform.pos - renderable.transformed_points[current_poly.vert[0]];
            current_poly.normal = line1.cross(line2).normalized();

            if (renderable.state & PolyAttribute::TwoSided) {
                if (current_poly.normal.dot(camera_ray) >= 0) {
                    auto proj_tri = camera_transform(renderable, current_poly, vp);
                    perspective_screen_transform(proj_tri, camera);

                    Math::V2D points[3];
                    points[0].x = proj_tri.p[0].pos.x;
                    points[0].y = proj_tri.p[0].pos.y;
                    points[1].x = proj_tri.p[1].pos.x;
                    points[1].y = proj_tri.p[1].pos.y;
                    points[2].x = proj_tri.p[2].pos.x;
                    points[2].y = proj_tri.p[2].pos.y;

                    m_rasterizer.draw_triangle(points);
                }
            } else {
                auto proj_tri = camera_transform(renderable, current_poly, vp);
                perspective_screen_transform(proj_tri, camera);

                Math::V2D points[3];
                points[0].x = proj_tri.p[0].pos.x;
                points[0].y = proj_tri.p[0].pos.y;
                points[1].x = proj_tri.p[1].pos.x;
                points[1].y = proj_tri.p[1].pos.y;
                points[2].x = proj_tri.p[2].pos.x;
                points[2].y = proj_tri.p[2].pos.y;

                m_rasterizer.draw_triangle(points);
            }
        }
    }
}

void RenderPipeline::perspective_screen_transform(Triangle &proj_tri, const Camera &camera) {
    float alpha = (0.5f * camera.width - 0.5f);
    float beta = (0.5f * camera.height - 0.5f);

    for (int vertex = 0; vertex < 3; vertex++) {
        float z = proj_tri.p[vertex].pos.z;
        proj_tri.p[vertex].pos.x = camera.view_dist_h * proj_tri.p[vertex].pos.x / z;
        proj_tri.p[vertex].pos.y =  camera.view_dist_v * proj_tri.p[vertex].pos.y * camera.aspect_ratio / z;

        proj_tri.p[vertex].pos.x = alpha + proj_tri.p[vertex].pos.x * alpha;
        proj_tri.p[vertex].pos.y = beta - proj_tri.p[vertex].pos.y * beta;
    }

}

