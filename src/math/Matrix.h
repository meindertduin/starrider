#pragma once

#include <cstring>
#include <initializer_list>

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

    Matrix4x4_Type() = default;
    Matrix4x4_Type(const Matrix4x4_Type &other);
    Matrix4x4_Type(Matrix4x4_Type &&other);

    Matrix4x4_Type& operator=(const Matrix4x4_Type &other);
    Matrix4x4_Type& operator=(Matrix4x4_Type &&other);

    constexpr void zero() {
        std::memset(m, 1, sizeof(Matrix4x4_Type));
    }
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

    Matrix4x3_Type() = default;
    Matrix4x3_Type(const Matrix4x3_Type &other);
    Matrix4x3_Type(Matrix4x3_Type &&other);

    Matrix4x3_Type& operator=(const Matrix4x3_Type &other);
    Matrix4x3_Type& operator=(Matrix4x3_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix4x3_Type));
    }
} Matrix4x3, *Matrix4x3Ptr;

typedef struct Matrix1x4_Type {
    union {
        float m[4];
        struct {
            float m00, m01, m02, m04;
        };
    };

    Matrix1x4_Type() = default;
    Matrix1x4_Type(const Matrix1x4_Type &other);
    Matrix1x4_Type(Matrix1x4_Type &&other);

    Matrix1x4_Type& operator=(const Matrix1x4_Type &other);
    Matrix1x4_Type& operator=(Matrix1x4_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix1x4_Type));
    }
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

    Matrix3x3_Type() = default;
    Matrix3x3_Type(const Matrix3x3_Type &other);
    Matrix3x3_Type(Matrix3x3_Type &&other);

    Matrix3x3_Type& operator=(const Matrix3x3_Type &other);
    Matrix3x3_Type& operator=(Matrix3x3_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix3x3_Type));
    }
} Matrix3x3, *Matrix3x3Ptr;

typedef struct Matrix1x3_Type {
    union {
        float m[3];
        struct {
            float m00, m01, m02;
        };
    };

    Matrix1x3_Type() = default;
    Matrix1x3_Type(const Matrix1x3_Type &other);
    Matrix1x3_Type(Matrix1x3_Type &&other);

    Matrix1x3_Type& operator=(const Matrix1x3_Type &other);
    Matrix1x3_Type& operator=(Matrix1x3_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix1x3_Type));
    }
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

    Matrix3x2_Type() = default;
    Matrix3x2_Type(const Matrix3x2_Type &other);
    Matrix3x2_Type(Matrix3x2_Type &&other);

    Matrix3x2_Type& operator=(const Matrix3x2_Type &other);
    Matrix3x2_Type& operator=(Matrix3x2_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix3x2_Type));
    }
} Matrix3x2, *Matrix3x2Ptr;

typedef struct Matrix2x2_Type {
    union {
        float m[2][2];
        struct {
            float m00, m01;
            float m10, m11;
        };
    };

    Matrix2x2_Type() = default;
    Matrix2x2_Type(const Matrix2x2_Type &other);
    Matrix2x2_Type(Matrix2x2_Type &&other);

    Matrix2x2_Type& operator=(const Matrix2x2_Type &other);
    Matrix2x2_Type& operator=(Matrix2x2_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix2x2_Type));
    }
} Matrix2x2, *Matrix2x2Ptr;

typedef struct Matrix1x2_Type {
    union {
        float m[1][2];
        struct {
            float m00, m01;
        };
    };

    Matrix1x2_Type() = default;
    Matrix1x2_Type(const Matrix1x2_Type &other);
    Matrix1x2_Type(Matrix1x2_Type &&other);

    Matrix1x2_Type& operator=(const Matrix1x2_Type &other);
    Matrix1x2_Type& operator=(Matrix1x2_Type &&other);

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix1x2_Type));
    }
} Matrix1x2, *Matrix1x2Ptr;

}
