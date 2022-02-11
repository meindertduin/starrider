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

        insert_object_render_list(object, render_list);

        light_camera_transform_object(object, vp, render_list);

        std::sort(render_list.begin(), render_list.end(), &render_polygon_avg_sort);

        for (auto render_poly : render_list) {
            perspective_screen_transform(camera, render_poly);
            draw_intensity_gouraud_triangle(render_poly);
            // draw_colored_gouraud_triangle(render_poly);
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
            object.transformed_vertices[i].n = mat_rot.transform(object.local_vertices[i].n);

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
            } else if(object.polygons[i].state & PolyStateBackface) {
                object.polygons[i].state ^= PolyStateBackface;
            }
        }
    }
}

void light_camera_transform_object(RenderObject &object, const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list) {
    for (auto &poly : render_list) {
        gourad_intensity_light_polygon(poly, g_lights, num_lights);

        camera_transform(vp, poly);
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

void gourad_light_polygon(Polygon &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r0_sum, g0_sum, b0_sum,
             r1_sum, g1_sum, b1_sum,
             r2_sum, g2_sum, b2_sum,
             ri, gi, bi,
             shaded_color;

    float dp, dist, n1, atten, i;

    if (polygon.attributes & PolyAttributeShadeModeGouraud) {
        r0_sum = g0_sum = b0_sum = 0;
        r1_sum = g1_sum = b1_sum = 0;
        r2_sum = g2_sum = b2_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                ri = ((lights[curr_light].c_ambient.r * polygon.color.r) >> 8);
                gi = ((lights[curr_light].c_ambient.g * polygon.color.g) >> 8);
                bi = ((lights[curr_light].c_ambient.b * polygon.color.b) >> 8);

                r0_sum += ri;
                g0_sum += gi;
                b0_sum += bi;

                r1_sum += ri;
                g1_sum += gi;
                b1_sum += bi;

                r2_sum += ri;
                g2_sum += gi;
                b2_sum += bi;
            }
            else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.vertices[polygon.vert[0]].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r0_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g0_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b0_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }

                dp = polygon.vertices[polygon.vert[1]].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r1_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g1_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b1_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }

                dp = polygon.vertices[polygon.vert[2]].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r2_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g2_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b2_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {
                auto l = V4D(polygon.vertices[polygon.vert[0]].v, lights[curr_light].pos);

                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r0_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                    g0_sum += ((lights[curr_light].c_diffuse.g * polygon.color.g * i) / (256 * 128));
                    b0_sum += ((lights[curr_light].c_diffuse.b * polygon.color.b * i) / (256 * 128));
                }
            }
        }

        if (r0_sum > 255) r0_sum = 255;
        if (g0_sum > 255) g0_sum = 255;
        if (b0_sum > 255) b0_sum = 255;
        if (r1_sum > 255) r1_sum = 255;
        if (g1_sum > 255) g1_sum = 255;
        if (b1_sum > 255) b1_sum = 255;
        if (r2_sum > 255) r2_sum = 255;
        if (g2_sum > 255) g2_sum = 255;
        if (b2_sum > 255) b2_sum = 255;

        polygon.lit_color[0] = RGBA(r0_sum, g0_sum, b0_sum, 0xFF);
        polygon.lit_color[1] = RGBA(r1_sum, g1_sum, b1_sum, 0xFF);
        polygon.lit_color[2] = RGBA(r2_sum, g2_sum, b2_sum, 0xFF);
    }
}

void gourad_intensity_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights) {
    uint32_t r_base, g_base, b_base,
             r0_sum,
             r1_sum,
             r2_sum,
             ri,
             shaded_color;

    float dp, dist, n1, atten, i;

    if (polygon.attributes & PolyAttributeShadeModeGouraud) {
        r0_sum = 0;
        r1_sum = 0;
        r2_sum = 0;

        for (int curr_light = 0; curr_light < max_lights; curr_light++) {
            if (!lights[curr_light].state) {
                continue;
            }

            if (lights[curr_light].attributes & LightAttributeAmbient) {
                ri = ((lights[curr_light].c_ambient.r * polygon.color.r) >> 8);

                r0_sum += ri;
                r1_sum += ri;
                r2_sum += ri;
            }
            else if (lights[curr_light].attributes & LightAttributeInfinite) {
                auto dp = polygon.trans_verts[0].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r0_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                }

                dp = polygon.trans_verts[1].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r1_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                }

                dp = polygon.trans_verts[2].n.dot(lights[curr_light].dir);

                if (dp > 0.0f) {
                    i = 128 * dp;

                    r2_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                }
            } else if (lights[curr_light].attributes * LightAttributePoint) {
                auto l = V4D(polygon.trans_verts[0].v, lights[curr_light].pos);

                dist = l.length();
                dp = polygon.normal.dot(l);

                if (dp > 0) {
                    atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);
                    i = 128 * dp / (polygon.normal.length() * dist * atten);

                    r0_sum += ((lights[curr_light].c_diffuse.r * polygon.color.r * i) / (256 * 128));
                }
            }
        }

        if (r0_sum > 255) r0_sum = 255;
        if (r1_sum > 255) r1_sum = 255;
        if (r2_sum > 255) r2_sum = 255;

        polygon.trans_verts[0].i = r0_sum / 255.0f;
        polygon.trans_verts[1].i = r1_sum / 255.0f;
        polygon.trans_verts[2].i = r2_sum / 255.0f;
    }
}

void flat_light_polygon(Polygon &polygon, Light *lights, int max_lights) {
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

        auto verts_color = RGBA(r_sum, g_sum, b_sum, 0xFF);

        // TODO: only change the poly color in light mode
        polygon.lit_color[0] = verts_color;
        polygon.lit_color[1] = verts_color;
        polygon.lit_color[2] = verts_color;
    }
}

}
