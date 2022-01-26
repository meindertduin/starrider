#pragma once

#include "Texture.h"
#include "../math/Vector.h"

using Math::Point4D;
using Math::V4D;

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
    RGBA_Type(uint32_t rgba);
    RGBA_Type(uint8_t r, uint8_t g, u_int8_t b, u_int8_t a) : r(r), g(g), b(b), a(a) {}
} RGBA;

constexpr uint32_t rgba_bit(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

enum MaterialState : uint16_t {
    TwoSided =  1,
    Transparent = 1 << 1,
    EightBitColor = 1 << 2,
    RGB16 = 1 << 3,
    RGB24 = 1 << 4,
    ShadeModeConstant = 1 << 5,
    ShadeModeEmmisive = 1 << 6,
    ShadeModeFlat = 1 << 7,
    ShadeModeGouraud = 1 << 8,
    ShateModeSimplePhong = 1 << 9,
    ShadeModeTexture = 1 << 10,
};

enum MaterialAttribute : uint16_t {
    Active = 1,
};

typedef struct Material_Type {
    int id;
    uint16_t state;
    uint16_t attributes;
    RGBA color;
    RGBA ra, rd, rs; // reflectivities
    Texture *texutre = nullptr;
} Material;

Material_Type materials[MaxMaterials];

enum LightAttribute : uint16_t {
    Ambient = 1,
    Infinite = 1 << 1,
    Point = 1 << 2,
    SpoitLight1 = 1 << 3,
    SpoitLight2 = 1 << 4,
};

enum LightState {
    On,
    Off,
};

static constexpr int MaxLights = 8;

typedef struct Light_Type {
    int id;
    LightState state;
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

Light lights[MaxLights];
int num_lights;

void reset_materials();

void reset_lights();

/* returns index on success and -1 on failure */
int init_light(int index,
        uint16_t attributes, LightState state,
        RGBA c_ambient, RGBA c_diffuse, RGBA c_specular,
        Point4D pos,
        V4D dir,
        float kc, float kl, float kq,
        float spot_inner, float spot_outer,
        float pf);

int create_base_ambient(int index, RGBA col);
int create_base_dir_light(int index, RGBA col, V4D pos);
