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

        constexpr void zero() {
            std::memset(m, 0, sizeof(Quat_Type));
        }
    } Quat, *Quat_Ptr;
}
