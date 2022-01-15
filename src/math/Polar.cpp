#include "Polar.h"
#include "Core.h"
#include <math.h>

namespace Math {
    Polar2D_Type::Polar2D_Type(const Point2D &p) {
        r = std::sqrt(p.x * p.x + p.y * p.y);
        theta = std::atan(p.x / p.y);
    }

    Point2D Polar2D_Type::to_point() const {
        Point2D p { r * fast_cos(theta), r * fast_sin(theta) };
        return p;
    }

    void Polar2D_Type::to_rect(float &x, float &y) const {
        x = r * fast_cos(theta);
        y = r * fast_sin(theta);
    }

    Cylindrical3D_Type::Cylindrical3D_Type(const Point3D &p) {
        r = std::sqrt(p.x * p.x + p.y * p.y);
        theta = std::atan(p.x / p.y);
        z = p.z;
    }

    Point3D Cylindrical3D::to_point() const {
        Point3D p { r * fast_cos(theta), r * fast_sin(theta), z};
        return p;
    }

    void Cylindrical3D::to_rect(float &x, float &y, float &z) const {
        x = r * fast_cos(theta);
        y = r * fast_sin(theta);
        z = this->z;
    }

    Spherical3D_Type::Spherical3D_Type(const Point3D &p) {
        rho = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
        theta = std::atan(p.y / p.x);
        phi = std::acos(p.z / rho);
    }

    Point3D Spherical3D::to_point() const {
        float z = rho * fast_sin(phi);
        Point3D p { (z * fast_cos(theta)), (z * fast_sin(theta)), z };
        return p;
    }

    void Spherical3D::to_rect(float &x, float &y, float &z) {
        z = rho * fast_sin(phi);
        x = z * fast_cos(theta);
        y = z * fast_sin(theta);
    }
}
