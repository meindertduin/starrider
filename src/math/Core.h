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
static constexpr int FixP16_DP_MKASK = 0x0000FFFF;
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

}
