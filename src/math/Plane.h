#pragma once

#include "Vector.h"

namespace Math {
    typedef struct Plane3D_Type {
        Point3D p0; // point of plane
        V3D n; // normal of the plane
    } Plane3D, *Plane3D_Ptr;
}
