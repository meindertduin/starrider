#pragma once

#include "Vector.h"

namespace Math {
const int PAR_LINE_NO_INTERSECT = 0;
const int PAR_LINE_INTERSECT_IN_SEGMENT = 1;
const int PAR_LINE_INTERSECT_OUT_SEGMENT = 2;
const int PAR_LINE_INTERSECT_EVERYWHERE = 3;

typedef struct ParLine2D_Type {
    Point2D p0; // start point
    Point2D p1; // end point
    V2D v; // direction of vector
} ParLine2D, *ParLine2D_Ptr;

typedef struct ParLine3D_Type {
    Point3D p0; // start point
    Point3D p1; // end point
    V3D v; // direction of vector
} ParLine3D, *ParLine3D_Ptr;

}
