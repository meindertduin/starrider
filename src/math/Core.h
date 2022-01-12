#pragma once

typedef int FixP16;
typedef int *FixP16_Ptr;

// pi consts
const float PI = 3.141592654f;
const float PI2 = 6.283185307f;
const float PI_DIV_2 = 1.570796327f;
const float PI_DIV_4 = 0.785398163f;
const float PI_INV = 0.318309886f;

// fixed point math consts
const int FixP16_SHIFT = 16;
const int FixP16_MAG = 65536;
const int FixP16_DP_MKASK = 0x0000FFFF;
const int FixP16_WP_MASK = 0xFFFF0000;
const int FixP16_ROUND_UP = 0x00008000;

const float EPSILON_E4 = 1E-4f;
const float EPSILON_E5 = 1E-5f;
const float EPSILON_E6 = 1E-6f;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))

#define SWAP(a, b, c) { c = a; a = b; b = c; }
#define DEG_TO_RAD(ang) ((ang) * PI / 180.0f)
#define RAD_TO_DEG(rads) ((rads) * 180.0f / PI)

#define RAND_RANGE(x, y) ((x) + rand()%((y) - (x) + 1))
