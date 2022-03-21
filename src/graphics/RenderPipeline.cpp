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

void RenderPipeline::render_objects(const Camera &camera, std::vector<RenderObject> renderables, RenderContext &context) {
    rast_set_frame_buffer(camera.width, camera.height, p_renderer->get_framebuffer());
    p_renderer->clear_screen();

    auto vp = camera.get_view_projection();
    camera_transform_lights(vp);

    context.render_list = std::vector<RenderListPoly>();

    for (auto object : renderables) {
        world_transform_object(object);

        backface_removal_object(object, camera);

        camera_trans_to_renderlist(object, vp, context);
    }

    frustrum_clip_renderlist(camera, context);

    light_renderlist(context);

    if (context.attributes & RCAttributeZSort) {
        std::sort(context.render_list.begin(), context.render_list.end(), &render_polygon_avg_sort);
    }

    perspective_screen_transform_renderlist(camera, context);

    draw_renderlist(context);
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

void frustrum_clip_renderlist(const Camera &camera, RenderContext &context) {
    for (auto &poly : context.render_list) {
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

                context.render_list.push_back(temp_poly);
            }
        }
    }
}

void camera_transform_lights(const Matrix4x4 &vp) {
    for (int i = 0; i < num_lights; i++) {
        if (g_lights[i].attributes == LightAttributePoint)
            g_lights[i].trans_pos = vp.transform(g_lights[i].pos);
    }
}

void light_renderlist(RenderContext &context) {
    for (auto &poly : context.render_list) {
        if (poly.state & PolyStateClipped) {
            continue;
        }

        gourad_intensity_light_polygon(poly, g_lights, num_lights);
        // flat_light_polygon(poly, g_lights, num_lights);
    }
}

void perspective_screen_transform_renderlist(const Camera &camera, RenderContext &context) {
    for (auto &poly : context.render_list) {
        if (poly.state & PolyStateClipped) {
            continue;
        }

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

void camera_trans_to_renderlist(RenderObject &object, const Matrix4x4 &vp, RenderContext &context) {
   if (!(object.state & ObjectStateActive) ||
           object.state & ObjectStateCulled ||
           !(object.state & ObjectStateVisible)) {
        return;
   }

   for (int poly = 0; poly < object.poly_count; poly++) {
        auto current_poly = object.polygons[poly];

        if (!(current_poly.state & PolyStateActive) ||
                current_poly.state & PolyStateBackface) {
            continue;
        }

        RenderListPoly render_poly = {
            .state = current_poly.state,
            .attributes = current_poly.attributes,
            .color = current_poly.color,
            .texture = current_poly.texture,
            .mati = current_poly.mati,
            .n_length = current_poly.n_length,
            .normal = current_poly.normal.normalized(),
            .verts = {
                object.local_vertices[current_poly.vert[0]],
                object.local_vertices[current_poly.vert[1]],
                object.local_vertices[current_poly.vert[2]],
            },
            .trans_verts = {
                object.transformed_vertices[current_poly.vert[0]],
                object.transformed_vertices[current_poly.vert[1]],
                object.transformed_vertices[current_poly.vert[2]],
            },
        };

        render_poly.trans_verts[0].i = current_poly.vertices[0].i;
        render_poly.trans_verts[1].i = current_poly.vertices[1].i;
        render_poly.trans_verts[2].i = current_poly.vertices[2].i;

        // TODO optimize
        render_poly.trans_verts[0].t = current_poly.text_coords[current_poly.text[0]];
        render_poly.trans_verts[1].t = current_poly.text_coords[current_poly.text[1]];
        render_poly.trans_verts[2].t = current_poly.text_coords[current_poly.text[2]];

        camera_transform(vp, render_poly);

        if (context.attributes & RCAttributeMipMapped) {
            int mip_level = (object.mip_levels * render_poly.trans_verts[0].v.z) / context.mip_z_dist;
            if (mip_level > object.mip_levels - 1) mip_level = object.mip_levels - 1;

            render_poly.texture = object.textures[mip_level];
        } else {
            render_poly.texture = object.textures[0];
        }


        context.render_list.push_back(render_poly);
   }
}

void draw_renderlist(RenderContext &context) {
    if (context.attributes & RCAttributeNoBuffer) {
        for (auto render_poly : context.render_list) {
            if (render_poly.state & PolyStateClipped) {
                continue;
            }

            if (render_poly.attributes & PolyAttributeShadeModeTexture) {
                // SHADE MODE FLAT
                if (render_poly.attributes & PolyAttributeShadeModeFlat) {
                    if (context.attributes & RCAttributeTextureHybrid) {
                        if (render_poly.trans_verts[0].v.z < context.perfect_dist) {
                            draw_perspective_textured_triangle_fs(render_poly);
                        } else if (render_poly.trans_verts[0].v.z > context.perfect_dist && render_poly.trans_verts[0].v.z < context.piecewise_dist) {
                            draw_piecewise_textured_triangle_fs(render_poly);
                        } else {
                            draw_affine_textured_triangle_fs(render_poly);
                        }
                    }
                    else if (context.attributes & RCAttributeTextureCorrect) {
                        draw_perspective_textured_triangle_fs(render_poly);
                    }
                    else if (context.attributes & RCAttributeTexturePiecewise) {
                        draw_piecewise_textured_triangle_fs(render_poly);
                    }
                    else if(context.attributes & RCAttributeTextureAffine) {
                        draw_affine_textured_triangle_fs(render_poly);
                    }
                }
                // SHADE MODE I GOURAD
                else if (render_poly.attributes & PolyAttributeShadeModeIntensityGourad) {
                    if (context.attributes & RCAttributeTextureHybrid) {
                        if (render_poly.trans_verts[0].v.z < context.perfect_dist) {
                            draw_perspective_textured_triangle_i(render_poly);
                        } else if (render_poly.trans_verts[0].v.z > context.perfect_dist && render_poly.trans_verts[0].v.z < context.piecewise_dist) {
                            draw_piecewise_textured_triangle_i(render_poly);
                        } else {
                            draw_affine_textured_triangle_i(render_poly);
                        }
                    }
                    else if (context.attributes & RCAttributeTextureCorrect) {
                        draw_perspective_textured_triangle_i(render_poly);
                    }
                    else if (context.attributes & RCAttributeTexturePiecewise) {
                        draw_piecewise_textured_triangle_i(render_poly);
                    }
                    else if(context.attributes & RCAttributeTextureAffine) {
                        draw_affine_textured_triangle_i(render_poly);
                    }
                }
            } else {
                // SHADE MODE SOLID
                if (render_poly.attributes & PolyAttributeShadeModeConstant) {
                    draw_triangle_s(render_poly);
                }
                else if (render_poly.attributes & PolyAttributeShadeModeFlat) {
                    draw_triangle_fs(render_poly);
                }
                else if (render_poly.attributes & PolyAttributeShadeModeIntensityGourad) {
                    draw_triangle_i(render_poly);
                }
            }
        }
    }

    else if (context.attributes & RCAttributeINVZBuffer) {
        for (auto render_poly : context.render_list) {
            if (render_poly.state & PolyStateClipped) {
                continue;
            }

            if (render_poly.attributes & PolyAttributeShadeModeTexture) {
                // SHADE MODE FLAT
                if (render_poly.attributes & PolyAttributeShadeModeFlat) {
                    if (context.attributes & RCAttributeTextureHybrid) {
                        if (render_poly.trans_verts[0].v.z < context.perfect_dist) {
                            draw_perspective_textured_triangle_fsinvzb(render_poly);
                        } else if (render_poly.trans_verts[0].v.z > context.perfect_dist && render_poly.trans_verts[0].v.z < context.piecewise_dist) {
                            draw_piecewise_textured_triangle_fsinvzb(render_poly);
                        } else {
                            draw_affine_textured_triangle_fsinvzb(render_poly);
                        }
                    }
                    else if (context.attributes & RCAttributeTextureCorrect) {
                        draw_perspective_textured_triangle_fsinvzb(render_poly);
                    }
                    else if (context.attributes & RCAttributeTexturePiecewise) {
                        draw_piecewise_textured_triangle_fsinvzb(render_poly);
                    }
                    else if(context.attributes & RCAttributeTextureAffine) {
                        draw_affine_textured_triangle_fsinvzb(render_poly);
                    }
                }
                // SHADE MODE I GOURAD
                else if (render_poly.attributes & PolyAttributeShadeModeIntensityGourad) {
                    if (context.attributes & RCAttributeTextureHybrid) {
                        if (render_poly.trans_verts[0].v.z < context.perfect_dist) {
                            draw_perspective_textured_triangle_iinvzb(render_poly);
                        } else if (render_poly.trans_verts[0].v.z > context.perfect_dist && render_poly.trans_verts[0].v.z < context.piecewise_dist) {
                            draw_piecewise_textured_triangle_iinvzb(render_poly);
                        } else {
                            draw_affine_textured_triangle_iinvzb(render_poly);
                        }
                    }
                    else if (context.attributes & RCAttributeTextureCorrect) {
                        draw_perspective_textured_triangle_iinvzb(render_poly);
                    }
                    else if (context.attributes & RCAttributeTexturePiecewise) {
                        draw_piecewise_textured_triangle_iinvzb(render_poly);
                    }
                    else if(context.attributes & RCAttributeTextureAffine) {
                        draw_affine_textured_triangle_iinvzb(render_poly);
                    }
                }
            } else {
                // SHADE MODE SOLID
                if (render_poly.attributes & PolyAttributeShadeModeConstant) {
                    draw_triangle_sinvzb(render_poly);
                }
                else if (render_poly.attributes & PolyAttributeShadeModeFlat) {
                    draw_triangle_fsinvzb(render_poly);
                }
                else if (render_poly.attributes & PolyAttributeShadeModeIntensityGourad) {
                    draw_triangle_iinvzb(render_poly);
                }
            }
        }
    }
}

}
