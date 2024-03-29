#pragma once

namespace Math {

struct Quat_Type;

typedef struct V2D_Type {
    union   {
        float m[2];
        struct {
            float x, y;
        };
    };

    V2D_Type() = default;
    constexpr V2D_Type(float x, float y) : x(x), y(y) {  }
    constexpr V2D_Type(const V2D_Type &init, const V2D_Type &term) : x(term.x - init.x), y(term.y - init.y) {}

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

    constexpr float dot(const V2D_Type &r) const {
        return x * r.x + y * r.y;
    }

    float length() const;
    V2D_Type& normalise();
    V2D_Type normalized() const;
    float cos_th(const V2D_Type &other) const;

} V2D, Point2D, *V2D_Ptr, *Point2D_Ptr;

typedef struct V3D_Type {
    union {
        float m[3];
        struct {
            float x, y, z;
        };
    };

    V3D_Type() = default;
    constexpr V3D_Type(float x, float y, float z) : x(x), y(y), z(z) {  }
    constexpr V3D_Type(const V3D_Type &init, const V3D_Type &term)
        : x(term.x - init.x), y(term.y - init.y), z(term.z - init.z) {}

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

    constexpr float dot(const V3D_Type &r) const {
        return x * r.x + y * r.y + z * r.z;
    }

    V3D_Type cross(const V3D_Type &other) const;
    float length() const;
    V3D_Type& normalise();
    V3D_Type normalized() const;
    float cos_th(const V3D_Type &other) const;

} V3D, Point3D, *V3D_Ptr, *Point3D_Ptr;

typedef struct V4D_Type {
    union {
        float m[4];
        struct {
            float x, y, z, w;
        };
    };

    constexpr V4D_Type() : x(0), y(0), z(0), w(1) {  };
    constexpr V4D_Type(float x, float y, float z) : x(x), y(y), z(z), w(1) {  }
    constexpr V4D_Type(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {  }
    constexpr V4D_Type(const V4D_Type &init, const V4D_Type &term)
        : x(term.x - init.x), y(term.y - init.y), z(term.z - init.z), w(1) {}

    V4D_Type operator+(const V4D_Type &rhs) const {
        V4D_Type r;
        r.x = x + rhs.x;
        r.y = y + rhs.y;
        r.z = z + rhs.z;

        return r;
    }

    V4D_Type operator-(const V4D_Type &rhs) const {
        V4D_Type r;
        r.x = x - rhs.x;
        r.y = y - rhs.y;
        r.z = z - rhs.z;

        return r;
    }

    V4D_Type operator*(float f) const {
        V4D_Type r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;

        return r;
    }

    V4D_Type operator/(float f) const {
        V4D_Type r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;

        return r;
    }

    V4D_Type& operator+=(const V4D_Type &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    V4D_Type& operator-=(const V4D_Type &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    V4D_Type& operator*=(float f) {
        x *=  f;
        y *= f;
        z *= f;

        return *this;
    }

    V4D_Type& operator/=(float f) {
        x /= f;
        y /= f;
        z /= f;

        return *this;
    }

    V4D_Type lerp(const V4D_Type &dest, float amt) {
        return (dest - *this) * amt + *this;
    }

    constexpr void zero() {
        x = y = z = w = 0.0f;
    }

    constexpr float dot(const V4D_Type &r) const {
        return x * r.x + y * r.y + z * r.z;
    }

    V4D_Type cross(const V4D_Type &other) const;
    float length() const;

    /* Really fast but has high error percentage */
    float length_fast() const;
    V4D_Type& normalise();
    V4D_Type normalized() const;
    float cos_th(const V4D_Type &other) const;
    V4D_Type rotate(const Quat_Type &rot);

} V4D, Point4D, *V4D_Ptr, *Point4D_Ptr;

typedef struct V2DI_Type {
    union {
        int m[2];
        struct {
            int x, y;
        };
    };
} V2DI, Point2DI;

typedef struct V3DI_Type {
    union {
        int m[3];
        struct {
            int x, y, z;
        };
    };
} V3DI, Point3DI;

typedef struct V4DI_Type {
    union {
        int m[4];
        struct {
            int x, y, z, w;
        };
    };
} V4DI, Point4DI;
}

