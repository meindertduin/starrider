#include "RenderObject.h"

int num_lights = 0;
Material_Type g_materials[MaxMaterials];
Light g_lights[MaxLights];

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

void reset_lights() {
    std::memset(g_lights, 0, MaxLights * sizeof(Light));
    num_lights = 0;
}

int init_light(int index,
        uint16_t attributes, uint16_t state,
        RGBA c_ambient, RGBA c_diffuse, RGBA c_specular,
        Point4D pos,
        V4D dir,
        float kc, float kl, float kq,
        float spot_inner, float spot_outer,
        float pf)
{
    if (index < 0 || index >= MaxLights) {
        return -1;
    }

    g_lights[index].state = state;
    g_lights[index].id = index;
    g_lights[index].attributes = attributes;
    g_lights[index].c_ambient = c_ambient;
    g_lights[index].c_diffuse = c_diffuse;
    g_lights[index].c_specular = c_specular;

    g_lights[index].kc = kc;
    g_lights[index].kl = kl;
    g_lights[index].kq = kq;

    g_lights[index].pos = pos;
    g_lights[index].dir = dir.normalized();

    g_lights[index].spot_inner = spot_inner;
    g_lights[index].spot_outer = spot_outer;
    g_lights[index].pf = pf;

    num_lights++;

    return index;
}

int create_base_amb_light(int index, RGBA col) {
    return init_light(index, LightAttributeAmbient, LightStateOn,
            col, RGBA { 0 }, RGBA { 0 },
            V4D(), V4D(),
            0, 0, 0,
            0, 0, 0);
}

int create_base_dir_light(int index, RGBA col, V4D dir) {
    return init_light(index, LightAttributeInfinite, LightStateOn,
            RGBA { 0 }, col, RGBA { 0 },
            V4D(), dir,
            0, 1, 0,
            0, 0, 0);
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
            .normal = current_poly.normal,
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

        render_list.push_back(render_poly);
   }
}

