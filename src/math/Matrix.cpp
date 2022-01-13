#include "Matrix.h"

namespace Math {

/* Copy and move constructors */

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
    std::memcpy(m, other.m, sizeof(Matrix4x4_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix4x3_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix1x4_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix3x3_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix1x3_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix3x2_Type));
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
    std::memcpy(m, other.m, sizeof(Matrix2x2_Type));
    return *this;
}

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
    std::memcpy(m, other.m, sizeof(Matrix1x2_Type));
    return *this;
}
}
