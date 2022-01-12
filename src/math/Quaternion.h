#pragma once

#include "Vector.h"

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
    } Quat, *Quat_Ptr;
}
