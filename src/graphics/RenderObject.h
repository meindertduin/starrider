#pragma once

#include "Core.h"
#include "../math/Vector.h"

using Math::V4D_Ptr;

typedef struct Polygon_Type {
    V4D_Ptr points_list;
    V4D_Ptr text_coords_list;
    int list_indices[3];
    V4D normal;
} Polygon;

typedef struct RenderObject_Type {
    int id;

    int vertex_count;
    Transform transform;

    V4D_Ptr local_points;
    V4D_Ptr transformed_points;
    V4D_Ptr text_coords_list;
} RenderObject;
