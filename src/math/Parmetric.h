#pragma once

#include "Vector.h"

namespace Math {

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
