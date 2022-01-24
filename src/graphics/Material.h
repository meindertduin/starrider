#pragma once

#include "Texture.h"

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
    Texture *texutre;
} Material;
