#pragma once

#include "Vector.h"
#include "Matrix.h"

#include <cstring>

namespace Math {
    typedef struct Quat_Type {
        union {
            float m[4];
            // TODO make this viable later on. If this is uncommented it get_instancea a
            // constructor not allowed in anonymous aggregate error.
            // struct {
            //     V3D_Type qv;
            //     float q0;
            // };
            struct {
                float x, y, z, w;
            };
        };

        Quat_Type() = default;
        constexpr Quat_Type(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {  }
        constexpr Quat_Type(const V3D_Type &v) : x(v.x), y(v.y), z(v.z), w(1.0f) {  }
        Quat_Type(const V4D_Type &axis, float angle);
        Quat_Type(const Matrix4x4 &rot);

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

        float length() const;
        Quat_Type normalized() const;
        Quat_Type conjugated() const;
        float dot(const Quat_Type &r) const;

        Matrix4x4_Type to_rotation_matrix() const;

        V4D_Type get_forward() const {
            return V4D_Type(0, 0, 1, 1).rotate(*this);
        }

        V4D_Type get_back() const {
            return V4D_Type(0, 0, -1, 1).rotate(*this);
        }

        V4D_Type get_right() const {
            return V4D_Type(1, 0, 0, 1).rotate(*this);
        }

        V4D_Type get_left() const {
            return V4D_Type(-1, 0, 0, 1).rotate(*this);
        }

        V4D_Type get_up() const {
            return V4D_Type(0, 1, 0, 1).rotate(*this);
        }

        V4D_Type get_down() const {
            return V4D_Type(0, -1, 0, 1).rotate(*this);
        }

        constexpr void zero() {
            std::memset(m, 0, sizeof(Quat_Type));
        }
    } Quat, *Quat_Ptr;
}
