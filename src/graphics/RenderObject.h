#pragma once

#include "Core.h"
#include "../math/Vector.h"
#include "Texture.h"

using Math::V4D_Ptr;

typedef struct Polygon_Type {
    V4D *points_list;
    V4D *text_coords;
    int vert[3];
    int text[3];
    V4D normal;
} Polygon;

enum ObjectState : uint16_t {
    BackfaceCulling = 1,
    SphereCulling = 1 << 1,
};

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
