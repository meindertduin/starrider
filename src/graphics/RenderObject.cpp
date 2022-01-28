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
