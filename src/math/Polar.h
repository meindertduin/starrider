#pragma once

namespace Math {
    typedef struct Polar2D_Type {
        float r; // radi of the point
        float theta; // angle in rads
    } Polar2D, *Polar2D_Ptr;

    typedef struct Cylindrical3D_Type {
        float r; // radi of point
        float theta; // angle degrees about the z-axis
        float z; // the z-height of point
    } Cylindrical3D, *Cylindrical3D_Ptr;

    typedef struct Spherical3D_Type {
        float p; // rho, the disantce to the point from origin
        float theta; // the angle from the z-axis of the line segment o->p
        float phi; // the angle from the projection if o->p onto the x-y plane and the x-axis
    } Spherical3D, *Spherical3D_Ptr;
}
