#pragma once

#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <memory>
#include <stdexcept>

#include "../math/Matrix.h"
#include "../math/Vector.h"
#include "../math/Quaternion.h"

float saturate(float val);

using Math::Matrix4x4;
using Math::V4D;
using Math::Quat_Type;

constexpr float fast_abs(float value) {
    return value > 0 ? value : -value;
}

typedef struct A565Color_Type {
    union {
        uint32_t value;
        struct {
            uint32_t alpha : 8;
            uint32_t noise : 8;
            uint32_t rgb_bit : 16;
        };
    };

    A565Color_Type() = default;
    constexpr A565Color_Type(uint32_t bit) : value(bit) { }

    A565Color_Type(int r, int g, int b) {
        value = (((b >> 3) & 31) + (((g >> 2) & 63) << 5) + (((b >> 3) & 31) << 11));
    }

    A565Color_Type(int r, int g, int b, int a) {
        value = ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11)) | a << 24;
    }

    constexpr void rgb565_from_16bit(uint32_t &r, uint32_t &g, uint32_t &b) {
        r = (value >> 11) & 31;
        g = (value >> 5) & 63;
        b = value & 31;
    }

    constexpr void rgb888_from_16bit(uint32_t &r, uint32_t &g, uint32_t &b) {
        r = ((value >> 11) & 31) << 3;
        g = ((value >> 5) & 63) << 2;
        b = (value & 31) << 3;
    }

    constexpr void rgba565_from_16bit(uint32_t &r, uint32_t &g, uint32_t &b, uint32_t &a) {
        a = (value >> 24) & 0xFF;
        r = (value >> 11) & 31;
        g = (value >> 5) & 63;
        b = value & 31;
    }

    constexpr uint32_t rgba_bit() const {
        uint32_t a = (value >> 24) & 0xFF;
        uint32_t r = (value >> 11) & 31;
        uint32_t g = (value >> 5) & 63;
        uint32_t b = value & 31;

        return (a << 24) | (r << 19) | (g << 10) | b << 3;
    }

    constexpr uint32_t rgba_bit(float f) const {
        uint32_t a = ((value >> 24) & 0xFF) * f;
        uint32_t r = ((value >> 11) & 31) * f;
        uint32_t g = ((value >> 5) & 63) * f;
        uint32_t b = (value & 31) * f;

        return (a << 24) | (r << 19) | (g << 10) | b << 3;
    }
} A565Color;

constexpr uint32_t rgb_from_565(int r, int g, int b) {
    return ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11));
}

constexpr void rgb565_from_16bit(uint32_t bit, uint32_t &r, uint32_t &g, uint32_t &b) {
    r = (bit >> 11) & 31;
    g = (bit >> 5) & 63;
    b = bit & 31;
}

constexpr void rgba565_from_16bit(uint32_t bit, uint32_t &a, uint32_t &r, uint32_t &g, uint32_t &b) {
    a = (bit >> 24) & 0xFF;
    r = (bit >> 11) & 31;
    g = (bit >> 5) & 63;
    b = bit & 31;
}

constexpr uint32_t rgba_bit(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

struct Color {
    float r, g, b;
    Color() {
        r = g = b = 0;
    }
    Color(float in_r, float in_g, float in_b) {
        r = in_r;
        g = in_g;
        b = in_b;
    }

    u_int32_t to_uint32() const {
        uint32_t rr = (uint32_t)(r * 255.0f);
    	uint32_t rg = (uint32_t)(g * 255.0f + 0.5f);
	    uint32_t rb = (uint32_t)(b * 255.0f + 0.5f);

        return (rr << 16 ) | (rg << 8) | rb;
    }

    Color operator-(const Color &c) const {
        return Color(r - c.r, g - c.g, b - c.b);
    }

    Color operator+(const Color &c) const {
        return Color(r + c.r, g + c.g, b + c.b);
    }

    Color& operator+=(const Color &c) {
        r += c.r;
        g += c.g;
        b += c.b;

        return *this;
    }

    Color& operator-=(const Color &c) {
        r -= c.r;
        g -= c.g;
        b -= c.b;

        return *this;
    }

    Color operator*(float f) const {
        return Color(r * f, g * f, b * f);
    }
};

struct Point {
    int x, y;
};

struct V2I {
    int x, y;

    V2I() {
        x = y = 0;
    }

    V2I(int x, int y) {
        this->x = x;
        this->y = y;
    }
};


struct Transform {
    V4D pos;
    Quat_Type rot;
    V4D scale;

    Transform() {
        this->pos = V4D(0, 0, 0, 0);
        this->rot = Quat_Type(0, 0, 0, 1);
        this->scale = V4D(1, 1, 1, 1);
    }

    Transform(const V4D &pos) {
        this->pos = pos;
        this->rot = Quat_Type(0, 0, 0, 1);
        this->scale = V4D(1, 1, 1, 1);
    }

    Transform(const V4D &pos, const Quat_Type &rot, const V4D &scale) {
        this->pos = pos;
        this->rot = rot;
        this->scale = scale;
    }

    void move(const V4D &dir, float amt) {
        set_pos(pos + (dir * amt));
    }

    Matrix4x4 get_matrix_transformation() const {
        Matrix4x4 translation, rotation, scale, identity;

        translation = Math::mat_4x4_translation(pos.x,  pos.y, pos.z);
        rotation = rot.conjugated().to_rotation_matrix();
        scale = Math::mat_4x4_scale(this->scale.x, this->scale.y, this->scale.z);

        return rotation * translation * scale;
    }

    Matrix4x4 get_rotation_matrix() const {
        return rot.conjugated().to_rotation_matrix();
    }

    void set_pos(const V4D &pos) {
        this->pos = pos;
    }

    Transform& rotate(const Quat_Type &rotation) {
        this->rot = (rotation * rot).normalized();

        return *this;
    }

    Transform& look_at(const V4D &point, const V4D &up) {
        auto mat_look_at = Math::mat_4x4_rotation(point, up);
        this->rot = Quat_Type(mat_look_at);

        return *this;
    }
};
