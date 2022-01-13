#pragma once

#include "Vector.h"
#include <cstring>

namespace Math {
    typedef struct Quat_Type {
        union {
            float m[4];
            struct {
                float q0;
                V3D qv;
            };
            struct {
                float x, y, z, w;
            };
        };

        Quat_Type() = default;
        constexpr Quat_Type(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {  }
        constexpr Quat_Type(const V3D &v) : x(v.x), y(v.y), z(v.z), w(1.0f) {  }

        Quat_Type(const Quat_Type &other);
        Quat_Type(Quat_Type &&other);

        Quat_Type& operator=(const Quat_Type &other);
        Quat_Type& operator=(Quat_Type &&other);

        Quat_Type operator*(const Quat_Type &q) const {
            Quat_Type r;
            r.w = w * q.w - x * q.x - y * q.y - z * q.z;
            r.x = x * q.w + w * q.x + y * q.z - z * q.y;
            r.y = y * q.w + w * q.y + z * q.x - x * q.z;
            r.z = z * q.w + w * q.z + x * q.y - y * q.x;

            return r;
        }

        Quat_Type operator*(const V4D_Type &v) const {
            Quat_Type r;
            r.w = -x * v.x - y * v.y - z * v.z;
            r.x =  w * v.x + y * v.z - z * v.y;
            r.y =  w * v.y + z * v.x - x * v.z;
            r.z =  w * v.z + x * v.y - y * v.x;

            return r;
        }

        Quat_Type operator*(float f) const {
            Quat_Type r;
            r.x = x * f;
            r.y = y * f;
            r.z = z * f;
            r.w = w * f;

            return r;
        }

        Quat_Type operator/(float f) const {
            Quat_Type r;
            r.x = x / f;
            r.y = y / f;
            r.z = z / f;
            r.w = w / f;

            return r;
        }

        constexpr void zero() {
            std::memset(m, 0, sizeof(Quat_Type));
        }
    } Quat, *Quat_Ptr;
}
