#include <list>

#include "RenderPipeline.h"
#include "../math/Core.h"

namespace Graphics {

enum ClipCodes : uint16_t {
    G = 0x0001,
    L = 0x0002,
    I = 0x0004,
};

RenderPipeline::RenderPipeline(Renderer *renderer) : p_renderer(renderer) {

}

void RenderPipeline::render_objects(const Camera &camera, std::vector<RenderObject> renderables) {
    rast_set_frame_buffer(camera.width, camera.height, p_renderer->get_framebuffer());
    p_renderer->clear_screen();

    auto vp = camera.get_view_projection();
    camera_transform_lights(vp);

    std::vector<RenderListPoly> render_list;

    for (auto object : renderables) {
        world_transform_object(object);

        backface_removal_object(object, camera);

        insert_object_render_list(object, render_list);
    }

    camera_transform_renderlist(vp, render_list);

    frustrum_clip_renderlist(camera, render_list);

    light_renderlist(render_list);

    std::sort(render_list.begin(), render_list.end(), &render_polygon_avg_sort);

    for (auto render_poly : render_list) {
        if (render_poly.state & PolyStateClipped) {
            continue;
        }

        perspective_screen_transform(camera, render_poly);

        // draw_piecewise_textured_triangle_iinvzb(render_poly);
        draw_perspective_textured_triangle_fsinvzb(render_poly);
        // draw_colored_gouraud_triangle(render_poly);
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

void frustrum_clip_renderlist(const Camera &camera, std::vector<RenderListPoly> &render_list) {
    for (auto &poly : render_list) {
        uint16_t vertex_clip_code[3];

        for (int i = 0; i < 3; i++) {
            auto max_x = poly.trans_verts[i].v.z * camera.tan_fov_div2;
            if (poly.trans_verts[i].v.x > max_x)
                vertex_clip_code[i] = ClipCodes::G;
            else if (poly.trans_verts[i].v.x < - max_x) {
                vertex_clip_code[i] = ClipCodes::L;
            } else {
                vertex_clip_code[i] = ClipCodes::I;
            }

        }

        if ((vertex_clip_code[0] == ClipCodes::G && vertex_clip_code[1] == ClipCodes::G && vertex_clip_code[2] == ClipCodes::G) ||
            (vertex_clip_code[0] == ClipCodes::L && vertex_clip_code[1] == ClipCodes::L && vertex_clip_code[2] == ClipCodes::L)) {
            poly.state |= PolyStateClipped;
            continue;
        }

        for (int i = 0; i < 3; i++) {
            auto max_y = poly.trans_verts[i].v.z * (camera.tan_fov_div2 / camera.aspect_ratio);
            if (poly.trans_verts[i].v.y > max_y)
                vertex_clip_code[i] = ClipCodes::G;
            else if (poly.trans_verts[i].v.y < - max_y) {
                vertex_clip_code[i] = ClipCodes::L;
            } else {
                vertex_clip_code[i] = ClipCodes::I;
            }
        }

        if ((vertex_clip_code[0] == ClipCodes::G && vertex_clip_code[1] == ClipCodes::G && vertex_clip_code[2] == ClipCodes::G) ||
            (vertex_clip_code[0] == ClipCodes::L && vertex_clip_code[1] == ClipCodes::L && vertex_clip_code[2] == ClipCodes::L)) {
            poly.state |= PolyStateClipped;
            continue;
        }

        int num_inside = 0;

        for (int i = 0; i < 3; i++) {
            if (poly.trans_verts[i].v.z > camera.m_zfar)
                vertex_clip_code[i] = ClipCodes::G;
            else if (poly.trans_verts[i].v.z < camera.m_znear) {
                vertex_clip_code[i] = ClipCodes::L;
            } else {
                vertex_clip_code[i] = ClipCodes::I;
                num_inside++;
            }
        }

        if ((vertex_clip_code[0] == ClipCodes::G && vertex_clip_code[1] == ClipCodes::G && vertex_clip_code[2] == ClipCodes::G) ||
            (vertex_clip_code[0] == ClipCodes::L && vertex_clip_code[1] == ClipCodes::L && vertex_clip_code[2] == ClipCodes::L)) {
            poly.state |= PolyStateClipped;
            continue;
        }

        int v0, v1, v2;
        V4D u, v, n;
        float t1, t2, xi, yi, ui, vi;
        float x01i, y01i, x02i, y02i, u01i, v01i, u02i, v02i;
        RenderListPoly temp_poly;

        // clip for one point inside
        if ((vertex_clip_code[0] | vertex_clip_code[1] | vertex_clip_code[2]) & ClipCodes::L) {
            // get the inside index
            if (num_inside == 1) {
                if (vertex_clip_code[0] == ClipCodes::I) {
                    v0 = 0; v1 = 1; v2 = 2;
                } else if (vertex_clip_code[1] == ClipCodes::I) {
                    v0 = 1; v1 = 2; v2 = 0;
                } else {
                    v0 = 2; v1 = 0; v2 = 1;
                }

                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v1].v);

                t1 = ((camera.m_znear - poly.trans_verts[v0].v.z) / v.z);
                xi = poly.trans_verts[v0].v.x + v.x * t1;
                yi = poly.trans_verts[v0].v.y + v.y * t1;

                poly.trans_verts[v1].v.x = xi;
                poly.trans_verts[v1].v.y = yi;
                poly.trans_verts[v1].v.z = camera.m_znear;

                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v2].v);

                t2 = ((camera.m_znear - poly.trans_verts[v0].v.z) / v.z);
                xi = poly.trans_verts[v0].v.x + v.x * t2;
                yi = poly.trans_verts[v0].v.y + v.y * t2;

                poly.trans_verts[v2].v.x = xi;
                poly.trans_verts[v2].v.y = yi;
                poly.trans_verts[v2].v.z = camera.m_znear;

                // TODO check if the poly is textured, for now we assume that every poly is textured
                ui = poly.trans_verts[v0].t.x + (poly.trans_verts[v1].t.x - poly.trans_verts[v0].t.x) * t1;
                vi = poly.trans_verts[v0].t.y + (poly.trans_verts[v1].t.y - poly.trans_verts[v0].t.y) * t1;
                poly.trans_verts[v1].t.x = ui;
                poly.trans_verts[v1].t.y = vi;

                ui = poly.trans_verts[v0].t.x + (poly.trans_verts[v2].t.x - poly.trans_verts[v0].t.x) * t2;
                vi = poly.trans_verts[v0].t.y + (poly.trans_verts[v2].t.y - poly.trans_verts[v0].t.y) * t2;
                poly.trans_verts[v1].t.x = ui;
                poly.trans_verts[v1].t.y = vi;

                u = V4D(poly.trans_verts[v0].v, poly.trans_verts[v1].v);
                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v2].v);

                // TODO optimize normalising
                n = u.normalized().cross(v.normalized());
                poly.n_length = 1;
                poly.normal = n;
            } else if (num_inside == 2) {
                temp_poly = poly;

                if (vertex_clip_code[0] == ClipCodes::L) {
                    v0 = 0; v1 = 1; v2 = 2;
                } else if (vertex_clip_code[1] == ClipCodes::L) {
                    v0 = 1; v1 = 2; v2 = 0;
                } else {
                    v0 = 2; v1 = 0; v2 = 1;
                }

                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v1].v);

                t1 = ((camera.m_znear - poly.trans_verts[v0].v.z) / v.z);
                x01i = poly.trans_verts[v0].v.x + v.x * t1;
                y01i = poly.trans_verts[v0].v.y + v.y * t1;

                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v2].v);

                t2 = ((camera.m_znear - poly.trans_verts[v0].v.z) / v.z);
                x02i = poly.trans_verts[v0].v.x + v.x * t2;
                y02i = poly.trans_verts[v0].v.y + v.y * t2;

                poly.trans_verts[v0].v.x = x01i;
                poly.trans_verts[v0].v.y = y01i;
                poly.trans_verts[v0].v.z = camera.m_znear;

                temp_poly.trans_verts[v1].v.x = x01i;
                temp_poly.trans_verts[v1].v.y = y01i;
                temp_poly.trans_verts[v1].v.z = camera.m_znear;

                temp_poly.trans_verts[v0].v.x = x02i;
                temp_poly.trans_verts[v0].v.y = y02i;
                temp_poly.trans_verts[v0].v.z = camera.m_znear;

                // TODO: check if poly is textured
                u01i = poly.trans_verts[v0].t.x + (poly.trans_verts[v1].t.x - poly.trans_verts[v0].t.x) * t1;
                v01i = poly.trans_verts[v0].t.y + (poly.trans_verts[v1].t.y - poly.trans_verts[v0].t.y) * t1;

                u02i = poly.trans_verts[v0].t.x + (poly.trans_verts[v2].t.x - poly.trans_verts[v0].t.x) * t2;
                v02i = poly.trans_verts[v0].t.y + (poly.trans_verts[v2].t.y - poly.trans_verts[v0].t.y) * t2;

                poly.trans_verts[v0].t.x = u01i;
                poly.trans_verts[v0].t.y = v01i;

                temp_poly.trans_verts[v0].t.x = u02i;
                temp_poly.trans_verts[v0].t.y = v02i;

                temp_poly.trans_verts[v1].t.x = u01i;
                temp_poly.trans_verts[v1].t.y = v01i;

                // Recalculate polys normals
                u = V4D(poly.trans_verts[v0].v, poly.trans_verts[v1].v);
                v = V4D(poly.trans_verts[v0].v, poly.trans_verts[v2].v);

                // TODO optimize normalising
                n = u.normalized().cross(v.normalized());
                poly.n_length = 1;
                poly.normal = n;

                u = V4D(temp_poly.trans_verts[v0].v, temp_poly.trans_verts[v1].v);
                v = V4D(temp_poly.trans_verts[v0].v, temp_poly.trans_verts[v2].v);

                // TODO optimize normalising
                n = u.normalized().cross(v.normalized());
                temp_poly.n_length = 1;
                temp_poly.normal = n;

                render_list.push_back(temp_poly);
            }
        }
    }
}


void camera_transform_renderlist(const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list) {
    for (auto &poly : render_list) {
        camera_transform(vp, poly);
    }
}

void camera_transform_lights(const Matrix4x4 &vp) {
    for (int i = 0; i < num_lights; i++) {
        if (g_lights[i].attributes == LightAttributePoint)
            g_lights[i].trans_pos = vp.transform(g_lights[i].pos);
    }
}

void light_renderlist(std::vector<RenderListPoly> &render_list) {
    for (auto &poly : render_list) {
        if (poly.state & PolyStateClipped) {
            continue;
        }

        // gourad_intensity_light_polygon(poly, g_lights, num_lights);
        flat_light_polygon(poly, g_lights, num_lights);
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
}
