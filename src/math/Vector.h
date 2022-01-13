#pragma once

namespace Math {

typedef struct V2D_Type {
    union   {
        float m[2];
        struct {
            float x, y;
        };
    };

    V2D_Type() = default;
    V2D_Type(float x, float y) : x(x), y(y) {  }

    V2D_Type operator+(const V2D_Type &rhs) const {
        V2D_Type r;
        r.x = x + rhs.x;
        r.y = y + rhs.y;

        return r;
    }

    V2D_Type operator-(const V2D_Type &rhs) const {
        V2D_Type r;
        r.x = x - rhs.x;
        r.y = y - rhs.y;

        return r;
    }

    V2D_Type operator*(float f) const {
        V2D_Type r;
        r.x = x * f;
        r.y = y * f;

        return r;
    }

    V2D_Type operator/(float f) const {
        V2D_Type r;
        r.x = x / f;
        r.y = y / f;

        return r;
    }

    V2D_Type& operator+=(const V2D_Type &rhs) {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    V2D_Type& operator-=(const V2D_Type &rhs) {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    V2D_Type& operator*=(float f) {
        x *=  f;
        y *= f;

        return *this;
    }

    V2D_Type& operator/=(float f) {
        x /= f;
        y /= f;

        return *this;
    }

    constexpr void zero() {
        x = y = 0.0f;
    }

} V2D, Point2D, *V2D_Ptr, *Point2D_Ptr;

typedef struct V3D_Type {
    union {
        float m[3];
        struct {
            float x, y, z;
        };
    };

    V3D_Type() = default;
    V3D_Type(float x, float y, float z) : x(x), y(y), z(z) {  }

    V3D_Type operator+(const V3D_Type &rhs) const {
        V3D_Type r;
        r.x = x + rhs.x;
        r.y = y + rhs.y;
        r.z = z + rhs.z;

        return r;
    }

    V3D_Type operator-(const V3D_Type &rhs) const {
        V3D_Type r;
        r.x = x - rhs.x;
        r.y = y - rhs.y;
        r.z = z - rhs.z;

        return r;
    }

    V3D_Type operator*(float f) const {
        V3D_Type r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;

        return r;
    }

    V3D_Type operator/(float f) const {
        V3D_Type r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;

        return r;
    }

    V3D_Type& operator+=(const V3D_Type &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    V3D_Type& operator-=(const V3D_Type &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    V3D_Type& operator*=(float f) {
        x *=  f;
        y *= f;
        z *= f;

        return *this;
    }

    V3D_Type& operator/=(float f) {
        x /= f;
        y /= f;
        z /= f;

        return *this;
    }

    constexpr void zero() {
        x = y = z = 0.0f;
    }
} V3D, Point3D, *V3D_Ptr, *Point3D_Ptr;

typedef struct V4D_Type {
    union {
        float m[4];
        struct {
            float x, y, z, w;
        };
    };

    V4D_Type() = default;
    constexpr V4D_Type(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {  }

    V4D_Type operator+(const V4D_Type &rhs) const {
        V4D_Type r;
        r.x = x + rhs.x;
        r.y = y + rhs.y;
        r.z = z + rhs.z;
        r.w = w + rhs.w;

        return r;
    }

    V4D_Type operator-(const V4D_Type &rhs) const {
        V4D_Type r;
        r.x = x - rhs.x;
        r.y = y - rhs.y;
        r.z = z - rhs.z;
        r.w = w - rhs.w;

        return r;
    }

    V4D_Type operator*(float f) const {
        V4D_Type r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;
        r.w = w * f;

        return r;
    }

    V4D_Type operator/(float f) const {
        V4D_Type r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;
        r.w = w / f;

        return r;
    }

    V4D_Type& operator+=(const V4D_Type &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;

        return *this;
    }

    V4D_Type& operator-=(const V4D_Type &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;

        return *this;
    }

    V4D_Type& operator*=(float f) {
        x *=  f;
        y *= f;
        z *= f;
        w *= f;

        return *this;
    }

    V4D_Type& operator/=(float f) {
        x /= f;
        y /= f;
        z /= f;
        w /= f;

        return *this;
    }

    constexpr void zero() {
        x = y = z = w = 0.0f;
    }
} V4D, Point4D, *V4D_Ptr, *Point4D_Ptr;

}
