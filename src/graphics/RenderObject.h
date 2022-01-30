#pragma once

#include "Core.h"
#include "../math/Vector.h"
#include "Texture.h"

using Math::Point4D;
using Math::V4D;

const uint16_t PolyStateActive = 1;
const uint16_t PolyStateClipped = 1 << 1;
const uint16_t PolyStateBackface = 1 << 2;

const uint16_t PolyAttributeTwoSided = 1;
const uint16_t PolyAttributeTransparent = 1 << 1;
const uint16_t PolyAttributeEightBitColor = 1 << 2;
const uint16_t PolyAttributeRGB16 = 1 << 3;
const uint16_t PolyAttributeRGB24 = 1 << 4;
const uint16_t PolyAttributeShadeModePure = 1 << 5;
const uint16_t PolyAttributeShadeModeConstant = 1 << 6;
const uint16_t PolyAttributeShadeModeFlat = 1 << 7;
const uint16_t PolyAttributeShadeModeGouraud = 1 << 8;
const uint16_t PolyAttributeShadeModeSimplePhong = 1 << 9;
const uint16_t PolyAttributeShadeModeTexture = 1 << 10;

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
} RGBA;

typedef struct Polygon_Type {
    uint16_t state;
    uint16_t attributes;
    V4D *points_list;
    V4D *text_coords;
    int vert[3];
    int text[3];
    V4D normal;
    RGBA color;
} Polygon;

typedef struct RenderObject_Type {
    int id;
    uint16_t state;

    Transform transform;

    Texture *texture;
    RGBA color;

    int vertex_count;
    V4D *local_points;
    V4D *transformed_points;

    int text_coords_count;
    V4D *text_coords;

    int poly_count;
    Polygon *polygons;

    RenderObject_Type(int id) : id(id) {}
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


struct RenderPolygon {
    uint16_t state;
    uint16_t attributes;
    V4D points[3];
    RGBA color;
    V4D *text_coords;
    int text[3];

    RenderPolygon(const Polygon &poly, RGBA color)
        : state(poly.state), attributes(poly.attributes), color(color), text_coords(poly.text_coords) {
            text[0] = poly.text[0];
            text[1] = poly.text[1];
            text[2] = poly.text[2];
        }
};


struct RenderListNode {
    RenderPolygon *data = nullptr;
    RenderListNode* next = nullptr;
};

struct RenderList {
    RenderListNode *head;

    int length;
    RenderList() : length(0), head(nullptr) {}
    ~RenderList() {
        while(head->next != nullptr) {
            auto next = head->next;
            delete head;
            head = next;
        }
    }

    void add_polygon(const Polygon &poly, RGBA color) {
        auto new_poly = new RenderPolygon { poly, color };
        auto node = new RenderListNode();

        node->data = new_poly;

        if (head == nullptr) {
            head = node;
        } else {
            RenderListNode *temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }

            temp->next = node;
        }

        length++;
    }

    void insertionSort() {
        RenderListNode* i = head->next;

        while (i != nullptr) {
            RenderListNode* key=i;
            RenderListNode* j=head;

            while (j!=i) {
                if (key->data->points[0].z > j->data->points[0].z) {
                    auto temp = key->data;
                    key->data = j->data;
                    j->data = temp;
                }
                j = j->next;

            }
            i = i->next;
        }
    }
};

