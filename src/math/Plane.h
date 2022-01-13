#pragma once

#include "Vector.h"
#include "Parmetric.h"

namespace Math {
    typedef struct Plane3D_Type {
        Point3D p0; // point of plane
        V3D n; // normal of the plane

        float point_in_plane(const Point3D &p) const;
        ParLine3D parm_line(const Point3D &p, float t) const;

    } Plane3D, *Plane3D_Ptr;
}
