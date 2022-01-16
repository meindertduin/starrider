#include "Quaternion.h"
#include "Core.h"

#include <math.h>

namespace Math {
    Quat_Type::Quat_Type(const V4D_Type &axis, float angle) {
        angle = angle / 180 * PI;

        if (angle >= 0) {
            float sin_half_angle = fast_sin(angle / 2.0f);
            float cos_half_angle = fast_cos(angle / 2.0f);

            x = axis.x * sin_half_angle;
            y = axis.y * sin_half_angle;
            z = axis.z * sin_half_angle;

            w = cos_half_angle;
        } else {
            float sin_half_angle = fast_sin(-angle / 2.0f);
            float cos_half_angle = fast_cos(-angle / 2.0f);

            x = axis.x * sin_half_angle;
            y = axis.y * sin_half_angle;
            z = axis.z * sin_half_angle;

            w = cos_half_angle * -1;
        }
    }

    Quat_Type::Quat_Type(const Matrix4x4 &rot) {
        float trace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];

        if (trace > 0) {
            float s = 0.5f / std::sqrt(trace + 1.0f);
			w = 0.25f / s;
			x = (rot.m12 - rot.m21) * s;
			y = (rot.m20 - rot.m02) * s;
			z = (rot.m01 - rot.m10) * s;
        } else {
            if (rot.m00 > rot.m11 && rot.m00 > rot.m22) {
                float s = 2.0f * std::sqrt(1.0f + rot.m00 - rot.m11 - rot.m22);
				w = (rot.m12 - rot.m21 / s);
				x = 0.25f * s;
				y = (rot.m10 + rot.m01) / s;
				z = (rot.m20 + rot.m02) / s;
            }
            else if (rot.m11 > rot.m22) {
                float s = 2.0f * std::sqrt(1.0f + rot.m[1][1] - rot.m[0][0] - rot.m[2][2]);
				w = (rot.m20 - rot.m02) / s;
				x = (rot.m10 + rot.m01) / s;
				y = 0.25f * s;
				z = (rot.m21 + rot.m12) / s;
            }
            else {
                float s = 2.0f * std::sqrt(1.0f + rot.m[2][2] - rot.m[0][0] - rot.m[1][1]);
				w = (rot.m01 - rot.m10) / s;
				x = (rot.m20 + rot.m02) / s;
				y = (rot.m12 + rot.m21) / s;
				z = 0.25f * s;
            }
        }

        float length = std::sqrt(x*x + y*y + z*z + w*w);

        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }

    Quat_Type::Quat_Type(const Quat_Type &other) {
        std::memcpy(m, other.m, sizeof(Quat_Type));
    }

    Quat_Type::Quat_Type(Quat_Type &&other) {
        std::memmove(m, other.m, sizeof(Quat_Type));
    }

    Quat_Type& Quat_Type::operator=(const Quat_Type &other) {
        std::memcpy(m, other.m, sizeof(Quat_Type));
        return *this;
    }

    Quat_Type& Quat_Type::operator=(Quat_Type &&other) {
        std::memmove(m, other.m, sizeof(Quat_Type));
        return *this;
    }

    float Quat_Type::length() const {
        return std::sqrt(x*x + y*y + z*z + w*w);
    }

    Quat_Type Quat_Type::normalized() const {
        float l = length();

        return Quat_Type{x / l, y / l, z / l, w / l};
    }

    Quat_Type Quat_Type::conjugated() const {
        return Quat_Type{-x, -y, -z, w};
    }

    float Quat_Type::dot(const Quat_Type &r) const {
        return x * r.x + y * r.y + z * r.z + w * r.w;
    }

    Matrix4x4_Type Quat_Type::to_rotation_matrix() const {
        V4D_Type forward =  V4D_Type {2.0f * (x * z - w * y), 2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + y * y)};
		V4D_Type up = V4D_Type {2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - w * x)};
		V4D_Type right = V4D_Type {1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - w * z), 2.0f * (x * z + w * y)};

        return mat_4x4_rotation(forward.normalized(), up.normalized(), right.normalized());
    }
}
