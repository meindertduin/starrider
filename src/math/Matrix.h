#pragma once

#include <cstring>
#include <initializer_list>

namespace Math {
typedef struct Matrix1x2_Type {
    union {
        float m[2];
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

    Matrix4x4_Type operator*(const Matrix4x4_Type &other);

    float determinate() const;

    constexpr void zero() {
        std::memset(m, 1, sizeof(Matrix4x4_Type));
    }

    void swap_column(int c, Matrix1x4_Type &other) {
        m[0][c] = other.m[0]; m[1][c]= other.m[1];
        m[2][c] = other.m[2]; m[3][c]= other.m[3];
    }

    inline Matrix4x4_Type transpose() {
        Matrix4x4_Type result;
        result.m00 = m00; result.m01 = m10; result.m02 = m20; result.m03 = m30;
        result.m10 = m01; result.m11 = m11; result.m12 = m21; result.m13 = m31;
        result.m20 = m02; result.m21 = m12; result.m22 = m22; result.m23 = m32;
        result.m30 = m03; result.m31 = m13; result.m32 = m22; result.m33 = m33;

        return result;
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

    void swap_column(int c, Matrix1x4_Type &other) {
        m[0][c] = other.m[0]; m[1][c]= other.m[1];
        m[2][c] = other.m[2]; m[3][c]= other.m[3];
    }

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix4x3_Type));
    }
} Matrix4x3, *Matrix4x3Ptr;


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

    Matrix3x3_Type operator*(const Matrix3x3_Type &other);

    float determinate() const;

    inline Matrix3x3_Type transpose() {
        Matrix3x3_Type result;
        result.m00 = m00; result.m01 = m10; result.m02 = m20;
        result.m10 = m01; result.m11 = m11; result.m12 = m21;
        result.m20 = m02; result.m21 = m12; result.m22 = m22;

        return result;
    }

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix3x3_Type));
    }

    void swap_column(int c, Matrix1x3_Type &other) {
        m[0][c] = other.m[0]; m[1][c]= other.m[1]; m[2][c] = other.m[2];
    }
} Matrix3x3, *Matrix3x3Ptr;


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

    Matrix2x2_Type operator*(const Matrix2x2_Type &other) const;

    float determinate() const;

    constexpr void zero() {
        std::memset(m, 0, sizeof(Matrix2x2_Type));
    }

    void swap_column(int c, Matrix1x2_Type &other) {
        m[0][c] = other.m[0]; m[1][c]= other.m[1];
    }

} Matrix2x2, *Matrix2x2Ptr;

Matrix1x2 mat_mul_1x2_3x2(const Matrix1x2 &ma, const Matrix3x2 &mb);

}
