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
            current_poly.attributes |= ShadeModeFlat;

            auto line1 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[1]];

            auto line2 = renderable.transformed_points[current_poly.vert[0]]
                - renderable.transformed_points[current_poly.vert[2]];

            auto camera_ray =  camera.m_transform.pos - renderable.transformed_points[current_poly.vert[0]].normalized();
            current_poly.normal = line1.cross(line2).normalized();

            if (renderable.state & PolyAttributeTwoSided) {
                // TODO: fix minor glitches in object polygons not being rendered
                // Probably has to do with the camera_ray not being entiterly accurate
                if (current_poly.normal.dot(camera_ray) >= 0.0f) {
                    auto poly_color = light_polygon(current_poly, camera, g_lights, num_lights);
                    V4D points[3];

                    camera_transform(renderable, vp, current_poly, points);
                    perspective_screen_transform(camera, points);

                    m_rasterizer.draw_triangle(points, poly_color);
                }
            } else {
                V4D points[3];
                auto poly_color = light_polygon(current_poly, camera, g_lights, num_lights);
                camera_transform(renderable, vp, current_poly, points);
                perspective_screen_transform(camera, points);

                m_rasterizer.draw_triangle(points, poly_color);
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

RGBA RenderPipeline::light_polygon(const Polygon &polygon, const Camera &camera, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r_sum, g_sum, b_sum,
             shaded_color;

    float dp, dist, i, n1, atten;

    if (polygon.attributes & ShadeModeFlat || polygon.attributes & ShadeModeGouraud) {
        r_sum = g_sum = b_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                r_sum += ((lights[curr_light].c_ambient.r * polygon.color.r) >> 8);
                g_sum += ((lights[curr_light].c_ambient.g * polygon.color.g) >> 8);
                b_sum += ((lights[curr_light].c_ambient.b * polygon.color.b) >> 8);
            } else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.normal.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    // TODO optimaize the normal length
                    i = 128 * dp / polygon.normal.length();

                    r_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {
                auto l = V4D(polygon.points_list[polygon.vert[0]], lights[curr_light].pos);

                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }
            }
        }

        if (r_sum > 255) r_sum = 255;
        if (g_sum > 255) g_sum = 255;
        if (b_sum > 255) b_sum = 255;

        return RGBA(r_sum, g_sum, b_sum, 0xFF);
    }

    return polygon.color;
}

