#pragma once

#include "Core.h"
#include "../math/Vector.h"
#include "Texture.h"

using Math::Point4D;
using Math::Point2D;
using Math::V4D;

const uint16_t ObjectStateNull = 0;
const uint16_t ObjectStateActive = 1;
const uint16_t ObjectStateVisible = 1 << 1;
const uint16_t ObjectStateCulled = 1 << 2;
const uint16_t ObjectStateLit = 1 << 3;

const uint16_t ObjectAttributeSingleFrame = 1;
const uint16_t ObjectAttributeMultiFrame = 1 << 1;
const uint16_t ObjectAttributeTextures = 1 << 2;

const uint16_t PolyStateNull = 0;
const uint16_t PolyStateActive = 1;
const uint16_t PolyStateClipped = 1 << 1;
const uint16_t PolyStateBackface = 1 << 2;
const uint16_t PolyStateLit = 1 << 3;

const uint16_t PolyAttributeTwoSided = 1;
const uint16_t PolyAttributeTransparent = 1 << 1;
const uint16_t PolyAttributeEightBitColor = 1 << 2;
const uint16_t PolyAttributeRGB16 = 1 << 3;
const uint16_t PolyAttributeRGB24 = 1 << 4;
const uint16_t PolyAttributeShadeModePure = 1 << 5;
const uint16_t PolyAttributeShadeModeConstant = 1 << 6;
const uint16_t PolyAttributeShadeModeFlat = 1 << 7;
const uint16_t PolyAttributeShadeModeGouraud = 1 << 8;
const uint16_t PolyAttributeShadeModeFastPhong = 1 << 9;
const uint16_t PolyAttributeShadeModeTexture = 1 << 10;
const uint16_t PolyAttributeEnableMaterial = 1 << 11;
const uint16_t PolyAttributeDisableMaterial = 1 << 12;

const uint16_t MaterialStateTransparent = 1 << 1;
const uint16_t MaterialStateEightBitColor = 1 << 2;
const uint16_t MaterialStateRGB16 = 1 << 3;
const uint16_t MaterialStateRGB24 = 1 << 4;
const uint16_t MaterialStateShadeModeConstant = 1 << 5;
const uint16_t MaterialStateShadeModeEmmisive = 1 << 6;
const uint16_t MaterialStateShadeModeFlat = 1 << 7;
const uint16_t MaterialStateShadeModeGouraud = 1 << 8;
const uint16_t MaterialStateShateModeSimplePhong = 1 << 9;
const uint16_t MaterialStateShadeModeTexture = 1 << 10;

const uint16_t MaterialAttributeActive = 1;

const uint16_t LightAttributeAmbient = 1;
const uint16_t LightAttributeInfinite = 1 << 1;
const uint16_t LightAttributePoint = 1 << 2;
const uint16_t LightAttributeSpoitLight1 = 1 << 3;
const uint16_t LightAttributeSpoitLight2 = 1 << 4;

const uint16_t ShadeModePure = 0x0000;
const uint16_t ShadeModeConstant = 0x0000;
const uint16_t ShadeModeFlat = 0x2000;
const uint16_t ShadeModeGouraud = 0x4000;
const uint16_t ShadeModePhong = 0x6000;
const uint16_t ShadeModeFastPhong = 0x6000;

const u_int16_t LightStateOn = 1;
const u_int16_t LightStateOff = 0;

const uint16_t VertexAttributeNull = 0;
const uint16_t VertexAttributePoint = 1 << 1;
const uint16_t VertexAttributeNormal = 1 << 2;
const uint16_t VertexAttributeTexture = 1 << 3;

const int ObjectMaxVertices = 4096;
const int ObjectMaxPolys = 8192;
const int RenderListMaxPolys = 32768;

static constexpr int MaxLights = 8;
static constexpr int MaxMaterials = 256;

typedef struct RGBA_Type {
    union {
        uint32_t rgba;
        uint8_t rgba_m[4];
        struct {
            u_int8_t a, b, g, r;
        };
    };

    RGBA_Type() = default;
    RGBA_Type(uint32_t rgba) : rgba(rgba) {  }
    RGBA_Type(uint8_t r, uint8_t g, u_int8_t b, u_int8_t a) : r(r), g(g), b(b), a(a) {}

    constexpr uint32_t to_argb_bit() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    constexpr uint32_t to_argb_bit(float i) {
        return ((uint32_t)(a * i) << 24) | ((uint32_t)(r * i) << 16) | ((uint32_t)(g * i) << 16) | ((uint32_t) (b * i));
    }
} RGBA;

typedef struct Vertext4D_Type {
    Point4D v;
    V4D n;
    Point2D t;
    float i;
    uint16_t attributes;
} Vertex4D;

typedef struct Polygon_Type {
    uint16_t state;
    uint16_t attributes;
    RGBA color;
    RGBA lit_color[3];

    int mati;
    Texture *texture;

    Vertex4D *vertices;
    Point2D *text_coords;

    int vert[3];
    int text[3];
    float n_length;
    // TODO can be deleted when using the RenderListPoly at this stage
    V4D normal;
} Polygon;

typedef struct RenderListPoly_Type {
    uint16_t state;
    uint16_t attributes;
    RGBA color;
    RGBA lit_color[3];
    Texture *texture;
    int mati;

    float n_length;
    V4D normal;

    float avg_z;
    Vertex4D verts[3];
    Vertex4D trans_verts[3];
} RenderListPoly;

typedef struct RenderObject_Type {
    int id;
    uint16_t state;
    uint16_t attributes;

    int mati;

    Transform transform;
    Texture *texture;
    RGBA color;

    int vertex_count;
    int frames_count;
    int text_count;
    int curr_frame;

    Vertex4D *local_vertices;
    Vertex4D *transformed_vertices;

    Vertex4D *head_local_vertices;
    Vertex4D *head_transformed_vertices;

    // TODO: check if these are still needed when we have
    // them stored in the vertex
    Point2D *texture_coords;

    int poly_count;
    Polygon *polygons;

    RenderObject_Type(int id) : id(id) {}

    int set_frame(int frame);
} RenderObject;

typedef struct Material_Type {
    int id;
    uint16_t state;
    uint16_t attributes;
    RGBA color;
    RGBA ra, rd, rs; // reflectivities
    Texture *texutre = nullptr;
} Material;

extern Material_Type g_materials[MaxMaterials];

typedef struct Light_Type {
    int id;
    uint16_t state;
    uint16_t attributes;

    RGBA c_ambient;
    RGBA c_diffuse;
    RGBA c_specular;
    Point4D pos;
    V4D dir;
    float kc, kl, kq;
    float spot_inner;
    float spot_outer;

    float pf;
} Light;

extern Light g_lights[MaxLights];

extern int num_lights;

void reset_materials();

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

constexpr uint32_t rgba_bit(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

constexpr bool render_polygon_avg_sort(const RenderListPoly &a, const RenderListPoly &b) {
    return 0.3333f * (a.trans_verts[0].v.z * a.trans_verts[1].v.z * a.trans_verts[2].v.z) >
        0.3333f * (b.trans_verts[0].v.z * b.trans_verts[1].v.z * b.trans_verts[2].v.z);
}

void insert_object_render_list(RenderObject &object, std::vector<RenderListPoly> &render_list);

