#pragma once

#include "Core.h"
#include "../math/Vector.h"
#include "Texture.h"

using Math::V4D_Ptr;

enum PolyState {
    Active = 1,
    Clipped = 1 << 1,
    Backface = 1 << 2,
};

enum PolyAttribute {
    TwoSided = 1,
    Transparent = 1 << 1,
    EightBitColor = 1 << 2,
    RGB16 = 1 << 3,
    RGB24 = 1 << 4,
    ShadeModePure = 1 << 5,
    ShadeModeConstant = 1 << 6,
    ShadeModeFlat = 1 << 7,
    ShadeModeGouraud = 1 << 8,
    ShadeModeSimplePhong = 1 << 9,
    ShadeModeTexture = 1 << 10,
};

typedef struct Polygon_Type {
    uint16_t state;
    uint16_t attributes;
    V4D *points_list;
    V4D *text_coords;
    int vert[3];
    int text[3];
    V4D normal;
} Polygon;

typedef struct RenderObject_Type {
    int id;
    uint16_t state;

    Transform transform;

    Texture *texture;

    int vertex_count;
    V4D *local_points;
    V4D *transformed_points;

    int text_coords_count;
    V4D *text_coords;

    int poly_count;
    Polygon *polygons;

    RenderObject_Type(int id) : id(id) {}
} RenderObject;
