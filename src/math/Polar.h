#pragma once

#include "Vector.h"

namespace Math {
    typedef struct Polar2D_Type {
        float r; // radi of the point
        float theta; // angle in rads

        Polar2D_Type() = default;
        constexpr Polar2D_Type(float r, float theta) : r(r), theta(theta) {  }

        Polar2D_Type(const Point2D &p);

        Point2D to_point() const;
        void to_rect(float &x, float &y) const;
    } Polar2D, *Polar2D_Ptr;

    typedef struct Cylindrical3D_Type {
        float r; // radi of point
        float theta; // angle degrees about the z-axis
        float z; // the z-height of point

        Cylindrical3D_Type() = default;
        constexpr Cylindrical3D_Type(float r, float theta, float z) : r(r), theta(theta), z(z) {  }
        Cylindrical3D_Type(const Point3D &p);

        Point3D to_point() const;
        void to_rect(float &x, float &y, float &z) const;
    } Cylindrical3D, *Cylindrical3D_Ptr;

    typedef struct Spherical3D_Type {
        float rho; // rho, the disantce to the point from origin
        float theta; // the angle from the z-axis of the line segment o->p
        float phi; // the angle from the projection if o->p onto the x-y plane and the x-axis

        Spherical3D_Type() = default;
        constexpr Spherical3D_Type(float rho, float theta, float phi) : rho(rho), theta(theta), phi(phi) {  }
        Spherical3D_Type(const Point3D &p);

        Point3D to_point() const;
        void to_rect(float &x, float &y, float &z);
    } Spherical3D, *Spherical3D_Ptr;
}
