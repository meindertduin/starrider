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

}
