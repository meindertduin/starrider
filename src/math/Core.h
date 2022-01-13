#pragma once

namespace Math {
typedef int FixP16;
typedef int *FixP16_Ptr;

// pi consts
static constexpr float PI = 3.141592654f;
static constexpr float PI2 = 6.283185307f;
static constexpr float PI_DIV_2 = 1.570796327f;
static constexpr float PI_DIV_4 = 0.785398163f;
static constexpr float PI_INV = 0.318309886f;

// fixed point math consts
static constexpr int FixP16_SHIFT = 16;
static constexpr int FixP16_MAG = 65536;
static constexpr int FixP16_DP_MASK = 0x0000FFFF;
static constexpr int FixP16_WP_MASK = 0xFFFF0000;
static constexpr int FixP16_ROUND_UP = 0x00008000;

static constexpr float EPSILON_E4 = 1E-4f;
static constexpr float EPSILON_E5 = 1E-5f;
static constexpr float EPSILON_E6 = 1E-6f;

template<typename T>
static constexpr T& min(const T &lhs, const T &rhs) {
    return lhs < rhs ? lhs : rhs;
}

template<typename T>
static constexpr T& max(const T &lhs, const T &rhs) {
    return lhs < rhs ? rhs : lhs;
}

static constexpr float deg_to_rad(const float ang) {
    return ang * PI / 180.0f;
}

static constexpr float rad_to_deg(const float rads) {
    return rads / 180.0f * PI;
}

static constexpr FixP16 FIXP16_WP(FixP16 f) {
    return f >> FixP16_SHIFT;
}

static constexpr FixP16 FixP16_DP(FixP16 f) {
    return f & FixP16_DP_MASK;
}

static constexpr FixP16 INT_TO_FIXP16(int i) {
    return i << FixP16_SHIFT;
}

static constexpr FixP16 FLOAT_TO_FIXP16(float f) {
    return f * FixP16_MAG + 0.5f;
}

static constexpr float FIXP16_TO_FLOAT(FixP16 f) {
    return static_cast<float>(f) / FixP16_MAG;
}

float fast_sin(float theta);
float fast_cos(float theta);

int fast_distance_2d(int x, int y);
float fast_distance_3d(float x, float y, float z);
}
