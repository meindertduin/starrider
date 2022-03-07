#include "RenderObject.h"

namespace Graphics {
static constexpr int MaxLights = 8;

typedef struct Light_Type {
    int id;
    uint16_t state;
    uint16_t attributes;

    RGBA c_ambient;
    RGBA c_diffuse;
    RGBA c_specular;

    Point4D pos;
    Point4D trans_pos;

    V4D dir;

    float kc, kl, kq;

    float pf;
} Light;

extern Light g_lights[MaxLights];
extern int num_lights;

void reset_lights();

/* returns index on success and -1 on failure */
int init_light(int index,
        uint16_t attributes, uint16_t state,
        RGBA c_ambient, RGBA c_diffuse, RGBA c_specular,
        Point4D pos,
        V4D dir,
        float kc, float kl, float kq,
        float spot_inner, float spot_outer,
        float pf);

int create_base_amb_light(int index, RGBA col);
int create_base_dir_light(int index, RGBA col, V4D dir);
int create_base_point_light(int index, RGBA col, V4D pos, float kc, float kl, float kq);

void gourad_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights);

void gourad_intensity_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights);

void flat_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights);
}

