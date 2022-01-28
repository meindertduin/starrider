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

            if (renderable.state & PolyAttributeTwoSided) {
                // TODO: fix minor glitches in object polygons not being rendered
                // Probably has to do with the camera_ray not being entiterly accurate
                if (current_poly.normal.dot(camera_ray) >= 0.0f) {
                    V4D points[3];
                    camera_transform(renderable, vp, current_poly, points);
                    perspective_screen_transform(camera, points);

                    m_rasterizer.draw_triangle(points, current_poly.color);
                }
            } else {
                V4D points[3];
                camera_transform(renderable, vp, current_poly, points);
                perspective_screen_transform(camera, points);

                m_rasterizer.draw_triangle(points, current_poly.color);
            }
        }
    }
}

void RenderPipeline::perspective_screen_transform(const Camera &camera, V4D *points) {
    float alpha = (0.5f * camera.width - 0.5f);
    float beta = (0.5f * camera.height - 0.5f);

    for (int vertex = 0; vertex < 3; vertex++) {
        float z = points[vertex].z;
        points[vertex].x = camera.view_dist_h * points[vertex].x / z;
        points[vertex].y =  camera.view_dist_v * points[vertex].y * camera.aspect_ratio / z;

        points[vertex].x = alpha + points[vertex].x * alpha;
        points[vertex].y = beta - points[vertex].y * beta;
    }
}

