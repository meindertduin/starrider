#include "Matrix.h"

namespace Math {

/* Copy and move constructors */

Matrix1x2_Type::Matrix1x2_Type(const Matrix1x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x2_Type));
}

Matrix1x2_Type::Matrix1x2_Type(Matrix1x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x2_Type));
}

Matrix1x2_Type& Matrix1x2_Type::operator=(const Matrix1x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x2_Type));
    return *this;
}

Matrix1x2_Type& Matrix1x2_Type::operator=(Matrix1x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x2_Type));
    return *this;
}

Matrix1x3_Type::Matrix1x3_Type(const Matrix1x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x3_Type));
}

Matrix1x3_Type::Matrix1x3_Type(Matrix1x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x3_Type));
}

Matrix1x3_Type& Matrix1x3_Type::operator=(const Matrix1x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x3_Type));
    return *this;
}

Matrix1x3_Type& Matrix1x3_Type::operator=(Matrix1x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x3_Type));
    return *this;
}

Matrix1x4_Type::Matrix1x4_Type(const Matrix1x4_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x4_Type));
}

Matrix1x4_Type::Matrix1x4_Type(Matrix1x4_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x4_Type));
}

Matrix1x4_Type& Matrix1x4_Type::operator=(const Matrix1x4_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix1x4_Type));
    return *this;
}

Matrix1x4_Type& Matrix1x4_Type::operator=(Matrix1x4_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix1x4_Type));
    return *this;
}

Matrix4x4_Type::Matrix4x4_Type(const Matrix4x4_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix4x4_Type));
}

Matrix4x4_Type::Matrix4x4_Type(Matrix4x4_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix4x4_Type));
}

Matrix4x4_Type& Matrix4x4_Type::operator=(const Matrix4x4_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix4x4_Type));
    return *this;
}

Matrix4x4_Type& Matrix4x4_Type::operator=(Matrix4x4_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix4x4_Type));
    return *this;
}

Matrix4x3_Type::Matrix4x3_Type(const Matrix4x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix4x3_Type));
}

Matrix4x3_Type::Matrix4x3_Type(Matrix4x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix4x3_Type));
}

Matrix4x3_Type& Matrix4x3_Type::operator=(const Matrix4x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix4x3_Type));
    return *this;
}

Matrix4x3_Type& Matrix4x3_Type::operator=(Matrix4x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix4x3_Type));
    return *this;
}

Matrix3x3_Type::Matrix3x3_Type(const Matrix3x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix3x3_Type));
}

Matrix3x3_Type::Matrix3x3_Type(Matrix3x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix3x3_Type));
}

Matrix3x3_Type& Matrix3x3_Type::operator=(const Matrix3x3_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix3x3_Type));
    return *this;
}

Matrix3x3_Type& Matrix3x3_Type::operator=(Matrix3x3_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix3x3_Type));
    return *this;
}

Matrix3x2_Type::Matrix3x2_Type(const Matrix3x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix3x2_Type));
}

Matrix3x2_Type::Matrix3x2_Type(Matrix3x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix3x2_Type));
}

Matrix3x2_Type& Matrix3x2_Type::operator=(const Matrix3x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix3x2_Type));
    return *this;
}

Matrix3x2_Type& Matrix3x2_Type::operator=(Matrix3x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix3x2_Type));
    return *this;
}

Matrix2x2_Type::Matrix2x2_Type(const Matrix2x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix2x2_Type));
}

Matrix2x2_Type::Matrix2x2_Type(Matrix2x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix2x2_Type));
}

Matrix2x2_Type& Matrix2x2_Type::operator=(const Matrix2x2_Type &other) {
    std::memcpy(m, other.m, sizeof(Matrix2x2_Type));
    return *this;
}

Matrix2x2_Type& Matrix2x2_Type::operator=(Matrix2x2_Type &&other) {
    std::memmove(m, other.m, sizeof(Matrix2x2_Type));
    return *this;
}

/* Matrix operator overloads */

Matrix2x2 Matrix2x2::operator*(const Matrix2x2_Type &other) const {
    Matrix2x2_Type r;
    r.m00 = m00 * other.m00 + m01 * other.m10;
    r.m01 = m00 * other.m01 + m01 * other.m11;

    r.m10 = m10 * other.m00 + m11 * other.m10;
    r.m11 = m10 * other.m01 + m11 * other.m11;

    return r;
}

Matrix3x3_Type Matrix3x3::operator*(const Matrix3x3_Type &other) {
    Matrix3x3_Type r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.m[j][i] = other.m[j][0] * m[0][i]
                + other.m[j][1] * m[1][i]
                + other.m[j][2] * m[2][i];
    return r;
}

Matrix4x4_Type Matrix4x4::operator*(const Matrix4x4_Type &other) {
    Matrix4x4_Type r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.m[j][i] = other.m[j][0] * m[0][i]
                + other.m[j][1] * m[1][i]
                + other.m[j][2] * m[2][i]
                + other.m[j][3] * m[3][i];
    return r;
}

/* Matrix multiplication functions */
Matrix1x2 mat_mul_1x2_3x2(const Matrix1x2 &ma, const Matrix3x2 &mb) {
    Matrix1x2 r;
    for (int col = 0; col < 2; ++col) {
        float sum = 0;
        int index = 0;
        while (index < 2) {
            sum += ma.m[index] * mb.m[index][col];
            index++;
        }

        sum += mb.m[index][col];
        r.m[col] = sum;
    }

    return r;
}

/* determinates */

float Matrix2x2::determinate() const {
    return m00 * m11 - m01 * m10;
}

float Matrix3x3::determinate() const {
    return m00 * (m11 * m22 - m21 * m12) - m01 * (m10 * m22 - m20 * m12)
        + m02 * (m10 * m21 - m20 * m11);
}

float Matrix4x4::determinate() const {
    // TODO implement the faster co-factor method. co-factors can significantly reduce
    // the amount of multiplications.
    return (m00 * m11 * m22 * m33) + (m01 * m12 * m23 * m30)
        + (m02 * m13 * m20 * m31) + (m03 * m10 * m21 * m32)
        - (m03 * m12 * m21 * m30) - (m02 * m11 * m20 * m33)
        - (m01 * m10 * m23 * m32) - (m00 * m13 * m22 * m31);
}

/* Inverses */

int Matrix2x2::inverse(Matrix2x2_Type &inv) const {
    auto det = determinate();
    if (det == 0) {
        return 0;
    }

    inv.m00 = m11 / det;
    inv.m01 = -m01 / det;
    inv.m10 = -m10 / det;
    inv.m11 = m00 / det;

    return 1;
}

int Matrix3x3_Type::inverse(Matrix3x3_Type &inv) const {
    auto det = determinate();

    if (det == 0) {
        return 0;
    }

    inv.m00 = (m11 * m22 - m21 * m12) / det;
    inv.m01 = (m02 * m21 - m01 * m22) / det;
    inv.m02 = (m01 * m12 - m02 * m11) / det;
    inv.m10 = (m12 * m20 - m10 * m22) / det;
    inv.m11 = (m00 * m22 - m02 * m20) / det;
    inv.m12 = (m10 * m02 - m00 * m12) / det;
    inv.m20 = (m10 * m21 - m20 * m11) / det;
    inv.m21 = (m20 * m01 - m00 * m21) / det;
    inv.m22 = (m00 * m11 - m10 * m01) / det;

    return 1;
}

int Matrix4x4::inverse(Matrix4x4_Type &inv) const {
    auto det = determinate();

    if (det == 0) {
        return 0;
    }

    det = 1.0f / det;

    float A2323 = m22 * m33 - m23 * m32;
    float A1323 = m21 * m33 - m23 * m31;
    float A1223 = m21 * m32 - m22 * m31;
    float A0323 = m20 * m33 - m23 * m30;
    float A0223 = m20 * m32 - m22 * m30;
    float A0123 = m20 * m31 - m21 * m30;
    float A2313 = m12 * m33 - m13 * m32;
    float A1313 = m11 * m33 - m13 * m31;
    float A1213 = m11 * m32 - m12 * m31;
    float A2312 = m12 * m23 - m13 * m22;
    float A1312 = m11 * m23 - m13 * m21;
    float A1212 = m11 * m22 - m12 * m21;
    float A0313 = m10 * m33 - m13 * m30;
    float A0213 = m10 * m32 - m12 * m30;
    float A0312 = m10 * m23 - m13 * m20;
    float A0212 = m10 * m22 - m12 * m20;
    float A0113 = m10 * m31 - m11 * m30;
    float A0112 = m10 * m21 - m11 * m20;

    inv.m00 =  (m11 * A2323 - m12 * A1323 + m13 * A1223) / det;
    inv.m01 = -(m01 * A2323 - m02 * A1323 + m03 * A1223) / det;
    inv.m02 =  (m01 * A2313 - m02 * A1313 + m03 * A1213) / det;
    inv.m03 = -(m01 * A2312 - m02 * A1312 + m03 * A1212) / det;
    inv.m10 = -(m10 * A2323 - m12 * A0323 + m13 * A0223) / det;
    inv.m11 =  (m00 * A2323 - m02 * A0323 + m03 * A0223) / det;
    inv.m12 = -(m00 * A2313 - m02 * A0313 + m03 * A0213) / det;
    inv.m13 =  (m00 * A2312 - m02 * A0312 + m03 * A0212) / det;
    inv.m20 =  (m10 * A1323 - m11 * A0323 + m13 * A0123) / det;
    inv.m21 = -(m00 * A1323 - m01 * A0323 + m03 * A0123) / det;
    inv.m22 =  (m00 * A1313 - m01 * A0313 + m03 * A0113) / det;
    inv.m23 = -(m00 * A1312 - m01 * A0312 + m03 * A0112) / det;
    inv.m30 = -(m10 * A1223 - m11 * A0223 + m12 * A0123) / det;
    inv.m31 =  (m00 * A1223 - m01 * A0223 + m02 * A0123) / det;
    inv.m32 = -(m00 * A1213 - m01 * A0213 + m02 * A0113) / det;
    inv.m33 =  (m00 * A1212 - m01 * A0212 + m02 * A0112) / det;

    return 1;
}

}
