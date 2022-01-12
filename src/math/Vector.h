#pragma once


namespace Math {

typedef struct V2D_Type {
    union   {
        float m[2];
        struct {
            float x, y;
        };
    };
} V2D, Point2D, *V2D_Ptr, *Point2D_Ptr;

typedef struct V3D_Type {
    union {
        float m[3];
        struct {
            float x, y, z;
        };
    };
} V3D, Point3D, *V3D_Ptr, *Point3D_Ptr;

typedef struct V4D_Type {
    union {
        float m[4];
        struct {
            float x, y, z, w;
        };
    };
} V4D, Point4D, *V4D_Ptr, *Point4D_Ptr;

}
