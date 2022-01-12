#pragma once

namespace Math {
typedef struct Matrix4x4_Type {
    union {
        float m[4][4];
        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };
} Matrix4x4, *Matrix4x4Ptr;

typedef struct Matrix4x3_Type {
    union {
        float m[4][3];
        struct {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
            float m30, m31, m32;
        };
    };
} Matrix4x3, *Matrix4x3Ptr;

typedef struct Matrix1x4_Type {
    union {
        float m[4];
        struct {
            float m00, m01, m02, m04;
        };
    };
} Matrix1x4, *Matrix1x4Ptr;

typedef struct Matrix3x3_Type {
    union {
        float m[3][3];
        struct {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
    };
} Matrix3x3, *Matrix3x3Ptr;

typedef struct Matrix1x3_Type {
    union {
        float m[3];
        struct {
            float m00, m01, m02;
        };
    };
} Matrix1x3, *Matrix1x3Ptr;

typedef struct Matrix3x2_Type {
    union {
        float m[3][2];
        struct {
            float m00, m01;
            float m10, m11;
            float m20, m21;
        };
    };
} Matrix3x2, *Matrix3x2Ptr;

typedef struct Matrix2x2_Type {
    union {
        float m[2][2];
        struct {
            float m00, m01;
            float m10, m11;
        };
    };
} Matrix2x2, *Matrix2x2Ptr;

typedef struct Matrix1x2_Type {
    union {
        float m[1][2];
        struct {
            float m00, m01;
        };
    };
} Matrix1x2, *Matrix1x2Ptr;

const Matrix4x4 IMATRIX_4X4 = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
};

const Matrix4x3 IMATRIX_4X3 = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    0, 0, 0,
};

const Matrix3x3 IMATRIX_3X3 {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
};

const Matrix3x3 IMATRIX_3X2 {
    1, 0,
    0, 1,
    0, 0,
};

const Matrix2x2 IMATRIX_2X2 {
    1, 0,
    0, 1,
};

const Matrix1x2 IMATRIX_1X2 {
    1, 0,
};

}
