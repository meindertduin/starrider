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
