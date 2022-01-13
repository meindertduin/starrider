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

    constexpr void zero() {
        x = y = z = w = 0.0f;
    }
} V4D, Point4D, *V4D_Ptr, *Point4D_Ptr;

}
