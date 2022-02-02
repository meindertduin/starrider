#include <list>

#include "RenderPipeline.h"

namespace Graphics {

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer) {

}

void RenderPipeline::render_objects(const Camera &camera, std::vector<RenderObject> renderables) {
    rast_set_frame_buffer(camera.width, camera.height, p_renderer->get_framebuffer());

    p_renderer->clear_screen();
    auto vp = camera.get_view_projection();

    auto v_forward = camera.m_transform.rot.get_back();

    for (auto object : renderables) {
        std::vector<RenderListPoly> render_list;

        world_transform_object(object);

        backface_removal_object(object, camera);

        light_camera_transform_object(object, vp, render_list);

        std::sort(render_list.begin(), render_list.end(), &render_polygon_avg_sort);

        for (auto render_poly : render_list) {
            perspective_screen_transform(camera, render_poly);
            draw_gouraud_triangle(render_poly);
        }
    }
}

void world_transform_object(RenderObject &object, CoordSelect coord_select) {
    if (!(object.state & ObjectStateActive) || !(object.state & ObjectStateVisible))
        return;

    Matrix4x4 mat_rot = object.transform.get_rotation_matrix();

    if (coord_select == CoordSelect::Local_To_Trans) {
        for (int i = 0; i < object.vertex_count; i++) {
            object.transformed_vertices[i].v = mat_rot.transform(object.local_vertices[i].v);

            object.transformed_vertices[i].v = object.transformed_vertices[i].v + object.transform.pos;

            object.transformed_vertices[i].v.x *= object.transform.scale.x;
            object.transformed_vertices[i].v.y *= object.transform.scale.y;
            object.transformed_vertices[i].v.z *= object.transform.scale.z;
        }
    } else if (coord_select == CoordSelect::Trans_Only) {
        for (int i = 0; i < object.vertex_count; i++) {
            object.transformed_vertices[i].v = mat_rot.transform(object.transformed_vertices[i].v);

            object.transformed_vertices[i].v = object.transformed_vertices[i].v + object.transform.pos;

            object.transformed_vertices[i].v.x *= object.transform.scale.x;
            object.transformed_vertices[i].v.y *= object.transform.scale.y;
            object.transformed_vertices[i].v.z *= object.transform.scale.z;
        }
    }
}

void backface_removal_object(RenderObject& object, const Camera &camera) {
    for (int i = 0; i < object.poly_count; i++) {
        auto line1 = object.transformed_vertices[object.polygons[i].vert[0]].v
            - object.transformed_vertices[object.polygons[i].vert[1]].v;

        auto line2 = object.transformed_vertices[object.polygons[i].vert[0]].v
            - object.transformed_vertices[object.polygons[i].vert[2]].v;

        auto camera_ray =  camera.m_transform.pos - object.transformed_vertices[object.polygons[i].vert[0]].v;
        object.polygons[i].normal = line1.cross(line2);

        if (object.state & PolyAttributeTwoSided) {
            if (object.polygons[i].normal.dot(camera_ray) < 0.0f) {
                object.polygons[i].state |= PolyStateBackface;
            } else {
                object.polygons[i].state = PolyStateNull;
            }
        }
    }
}

void light_camera_transform_object(RenderObject &object, const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list) {
    for (int j = 0; j < object.poly_count; j++) {
        auto current_poly = object.polygons[j];

        if (!(object.polygons[j].state & PolyStateBackface)) {
            auto poly_color = light_polygon(current_poly, g_lights, num_lights);

            RenderListPoly render_poly = {
                .state = current_poly.state,
                .attributes = current_poly.attributes,
                .color = poly_color,
                .texture = current_poly.texture,
                .mati = current_poly.mati,
                .n_length = current_poly.n_length,
                .normal = current_poly.normal,
                .verts = {
                    current_poly.vertices[current_poly.vert[0]],
                    current_poly.vertices[current_poly.vert[1]],
                    current_poly.vertices[current_poly.vert[2]],
                },
                .trans_verts = {
                    object.transformed_vertices[current_poly.vert[0]],
                    object.transformed_vertices[current_poly.vert[1]],
                    object.transformed_vertices[current_poly.vert[2]],
                },
            };

            camera_transform(object, vp, current_poly, render_poly);
            render_list.push_back(render_poly);
        }
    }
}


void perspective_screen_transform(const Camera &camera, RenderListPoly &poly) {
    float alpha = (0.5f * camera.width - 0.5f);
    float beta = (0.5f * camera.height - 0.5f);

    for (int vertex = 0; vertex < 3; vertex++) {
        float z = poly.trans_verts[vertex].v.z;
        poly.trans_verts[vertex].v.x = camera.view_dist_h * poly.trans_verts[vertex].v.x / z;
        poly.trans_verts[vertex].v.y =  camera.view_dist_v * poly.trans_verts[vertex].v.y * camera.aspect_ratio / z;

        poly.trans_verts[vertex].v.x = alpha + poly.trans_verts[vertex].v.x * alpha;
        poly.trans_verts[vertex].v.y = beta - poly.trans_verts[vertex].v.y * beta;
    }
}

RGBA light_polygon(const Polygon &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r_sum, g_sum, b_sum,
             shaded_color;

    float dp, dist, i, n1, atten;

    if (polygon.attributes & PolyAttributeShadeModeFlat || polygon.attributes & PolyAttributeShadeModeGouraud) {
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
                auto l = V4D(polygon.vertices[polygon.vert[0]].v, lights[curr_light].pos);

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

}
