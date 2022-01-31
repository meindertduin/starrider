#include "Vector.h"
#include <math.h>
#include "Core.h"
#include "Quaternion.h"

namespace Math {
    float V2D_Type::length() const {
        return std::sqrt(x * x + y * y);
    }

    V2D_Type& V2D_Type::normalise() {
        float l = length();
        x /= l; y /= l;

        return *this;
    }

    V2D_Type V2D_Type::normalized() const {
        float l = length();
        V2D_Type r { x / l, y / l };
        return r;
    }


    float V2D_Type::cos_th(const V2D_Type &other) const {
        return fast_cos(dot(other) / (length() * other.length()));
    }

    V3D_Type V3D_Type::cross(const V3D_Type &other) const {
        V3D_Type r;
        r.x = y * other.z - z * other.y;
        r.y = z * other.x - x * other.z;
        r.z = x * other.y - y * other.x;

        return r;
    }

    float V3D_Type::length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    V3D_Type& V3D_Type::normalise() {
        float l = length();
        x /= l; y /= l; z /= l;

        return *this;
    }

    V3D_Type V3D_Type::normalized() const {
        float l = length();
        V3D_Type r { x / l, y / l, z / l };
        return r;
    }

    float V3D_Type::cos_th(const V3D_Type &other) const {
        return fast_cos(dot(other) / (length() * other.length()));
    }

    V4D_Type V4D_Type::cross(const V4D_Type &other) const {
        V4D_Type r;
        r.x = y * other.z - z * other.y;
        r.y = z * other.x - x * other.z;
        r.z = x * other.y - y * other.x;
        r.w = 1.0f;

        return r;
    }

    float V4D_Type::length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float V4D_Type::length_fast() const {
        int temp;
        int xi, yi, zi;

        xi = Math::fabs(this->x) * 1024;
        yi = Math::fabs(this->y) * 1024;
        zi = Math::fabs(this->z) * 1024;

        if (yi < xi) {
            temp = xi;
            xi = yi;
            yi = temp;
        }

        if (zi < yi) {
            temp = zi;
            zi = yi;
            yi = temp;
        }

        if (yi < xi) {
            temp = xi;
            xi = yi;
            yi = temp;
        }

        int dist = (zi + 11 * (yi >> 5) + (xi >> 2));
        return (float)(dist >> 10);
    }

    V4D_Type& V4D_Type::normalise() {
        float l = length();
        x /= l; y /= l; z /= l;
        return *this;
    }

    V4D_Type V4D_Type::normalized() const {
        float l = length();
        return V4D_Type { x / l, y / l, z / l, w };
    }

    float V4D_Type::cos_th(const V4D_Type &other) const {
        return fast_cos(dot(other) / (length() * other.length()));
    }

    V4D_Type V4D_Type::rotate(const Quat_Type &rot) {
        Quat_Type conj = rot.conjugated();
        Quat_Type new_rot = rot * *this * conj;

        x = new_rot.x;
        y = new_rot.y;
        z = new_rot.z;
        w = 1.0f;

        return *this;
    }
}
