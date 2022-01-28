#include "RenderObject.h"

int num_lights = 8;
Material_Type materials[MaxMaterials];
Light lights[MaxLights];

void reset_materials() {
    static int first_time = 1;
    if (first_time) {
        std::memset(materials, 0, MaxMaterials * sizeof(Material_Type));
        first_time = 0;
    }

    for (int curr_mat = 0; curr_mat < MaxMaterials; curr_mat++) {
        delete materials[curr_mat].texutre;
        std::memset(&materials[curr_mat], 0, sizeof(Material_Type));
    }
}

void reset_lights() {
    std::memset(lights, 0, MaxLights * sizeof(Light));
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
    if (index < 0 || index <= MaxLights) {
        return -1;
    }

    lights[index].state = state;
    lights[index].id = index;
    lights[index].attributes = attributes;
    lights[index].c_ambient = c_ambient;
    lights[index].c_diffuse = c_diffuse;
    lights[index].c_specular = c_specular;

    lights[index].kc = kc;
    lights[index].kl = kl;
    lights[index].kq = kq;

    lights[index].pos = pos;
    lights[index].dir = dir.normalized();

    lights[index].spot_inner = spot_inner;
    lights[index].spot_outer = spot_outer;
    lights[index].pf = pf;

    return index;
}

int create_base_amb_light(int index, RGBA col) {
    return init_light(index, LightAttributeAmbient, LightStateOn,
            col, RGBA { 0 }, RGBA { 0 },
            V4D(), V4D(),
            0, 0, 0,
            0, 0, 0);
}

int create_base_dir_light(int index, RGBA col, V4D pos) {
    return init_light(index, LightAttributeAmbient, LightStateOn,
            RGBA { 0 }, col, RGBA { 0 },
            pos, V4D(),
            0, 1, 0,
            0, 0, 0);
}
