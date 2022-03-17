#include "RenderObject.h"

Material_Type g_materials[MaxMaterials];

void reset_materials() {
    static int first_time = 1;
    if (first_time) {
        std::memset(g_materials, 0, MaxMaterials * sizeof(Material_Type));
        first_time = 0;
    }

    for (int curr_mat = 0; curr_mat < MaxMaterials; curr_mat++) {
        delete g_materials[curr_mat].texutre;
        std::memset(&g_materials[curr_mat], 0, sizeof(Material_Type));
    }
}

int RenderObject_Type::set_frame(int frame) {
    if (!(this->attributes & ObjectAttributeMultiFrame))
        return 0;

    if (frame < 0) {
        frame = 0;
    } else {
        frame = this->frames_count - 1;
    }

    this->curr_frame = frame;

    this->local_vertices = &(this->head_local_vertices[frame * this->vertex_count]);
    this->transformed_vertices = &(this->head_transformed_vertices[frame * this->vertex_count]);

    return 1;
}

void insert_object_render_list(RenderObject &object, std::vector<RenderListPoly> &render_list) {
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

        int mip_level = (object.mip_levels * current_poly.vertices[0].v.z) / 20;
        if (mip_level > object.mip_levels) mip_level = object.mip_levels;

        for (int ts = 0; ts < mip_level; ts++) {
            render_poly.trans_verts[0].t.x *= 0.5f;
            render_poly.trans_verts[0].t.y *= 0.5f;

            render_poly.trans_verts[1].t.x *= 0.5f;
            render_poly.trans_verts[1].t.y *= 0.5f;

            render_poly.trans_verts[2].t.x *= 0.5f;
            render_poly.trans_verts[2].t.y *= 0.5f;
        }

        render_poly.texture = object.textures[1];

        // TODO optimize
        render_poly.trans_verts[0].t = current_poly.text_coords[current_poly.text[0]];
        render_poly.trans_verts[1].t = current_poly.text_coords[current_poly.text[1]];
        render_poly.trans_verts[2].t = current_poly.text_coords[current_poly.text[2]];

        render_list.push_back(render_poly);
   }
}

