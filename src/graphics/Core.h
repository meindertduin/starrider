#pragma once

#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <memory>

#include "../io/BmpReader.h"
#include "Texture.h"
#include "../math/Matrix.h"
#include "../math/Vector.h"

class Texture;

float saturate(float val);

using Math::Matrix4x4;
using Math::V4D;


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

struct Quaternion;


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

struct V4F {
    float x, y, z, w;
    V4F() {
        x = y = z = 0;
        w = 1;
    }

    V4F(const float &in_x, const float &in_y, const float &in_z) {
        x = in_x;
        y = in_y;
        z = in_z;
        w = 1;
    }

    V4F(const float &in_x, const float &in_y, const float &in_z, const float &in_w) {
        x = in_x;
        y = in_y;
        z = in_z;
        w = in_w;
    }

    V4F lerp(V4F dest, float amt) {
        return (dest - *this) * amt + *this;
    }

    float length() const {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    void normalise() {
        float l = length();
        x /= l; y /= l; z /= l;
    }

    V4F normalized() const {
        V4F r = *this;
        r.normalise();
        return r;
    }

    float dot(const V4F &r) const {
        return x * r.x + y * r.y + z * r.z + w * r.w;
    }

    float prod(const V4F& b) const {
        return (x * b.x + y * b.y + z * b.z);
    }

    V4F cross(const V4F &v) {
        V4F r;
        r.x = y * v.z - z * v.y;
        r.y = z * v.x - x * v.z;
        r.z = x * v.y - y * v.x;
        r.w = 1.0f;

        return r;
    }

    V4F normal() {
        return *this / length();
    }

    V4F& rotate(const Quaternion &rot);

    V4F operator+(const V4F& rhs) const {
        V4F r;
        r.x = this->x + rhs.x;
        r.y = this->y + rhs.y;
        r.z = this->z + rhs.z;
        r.w = this->w + rhs.w;
        return r;
    }

    V4F operator-(const V4F& rhs) const {
        V4F r;
        r.x = this->x - rhs.x;
        r.y = this->y - rhs.y;
        r.z = this->z - rhs.z;
        r.w = this->w - rhs.w;
        return r;
    }

    V4F operator*(float f) const {
        V4F r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;
        r.w = w * f;

        return r;
    }

    V4F operator/(float f) const {
        V4F r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;
        r.w = w / f;

        return r;
    }

    V4F& operator*=(float f) {
        x = x * f;
        y = y * f;
        z = z * f;
        w = w * f;
        return *this;
    }

    V4F& operator/=(float f) {
        x = x / f;
        y = y / f;
        z = z / f;
        w = w / f;
        return *this;
    }

    // TODO delete this
    V4D to_v4d() const {
        return V4D {
            x, y, z, w
        };
    }
};

struct Vertex {
    V4F pos;
    V4F text_coords;
    V4F normal;

    Vertex() {}

    Vertex(float x, float y, float z) {
        pos = V4F(x, y, z);
    }

    Vertex(float x, float y, float z, float w) {
        pos = V4F(x, y, z, w);
    }

    Vertex(const V4F &in_pos, const V4F &in_text_coords, const V4F &in_normal) {
        pos = in_pos;
        text_coords = in_text_coords;
        normal = in_normal;
    }

    Vertex transform(const Matrix4x4 &m);
    Vertex transform(const Matrix4x4 &transform, const Matrix4x4 &normal);
    void normal_transform(const Matrix4x4 &normal_matrix);

    Vertex& perspective_divide() {
        if (pos.w != 0.0) {
            pos.x /= pos.w; pos.y /= pos.w; pos.z/= pos.w;
        }

        return *this;
    }

    Vertex lerp(const Vertex &v, float amt) {
        return Vertex(pos.lerp(v.pos, amt), text_coords.lerp(v.text_coords, amt), normal.lerp(v.normal, amt));
    }

    bool inside_view_frustrum() const {
        return std::abs(pos.x) <= std::abs(pos.w) &&
            std::abs(pos.y) <= std::abs(pos.w) &&
            std::abs(pos.z) <= std::abs(pos.w);
    }

    float get(int index) {
        switch(index) {
            case 0:
                return pos.x;
            case 1:
                return pos.y;
            case 2:
                return pos.z;
            case 3:
                return pos.w;
            default:
                // TODO maybe throw error?
                return pos.w;
        }
    }

    Vertex operator+(const Vertex &v) const {
        Vertex r;
        r.pos = pos + v.pos;
        return r;
    }

    Vertex operator-(const Vertex &v) const {
        Vertex r;
        r.pos = pos + v.pos;
        return r;
    }

    Vertex operator*(float f) const {
        Vertex r;
        r.pos *= f;
        return r;
    }

    Vertex operator/(float f) const {
        Vertex r;
        r.pos /= f;
        return r;
    }
};

struct Gradients {
    V4F text_coords[3];
    float one_over_z[3];
    float depth[3];
    float light_amount[3];

    float text_coord_x_xstep;
    float text_coord_x_ystep;

    float text_coord_y_xstep;
    float text_coord_y_ystep;

    float one_over_zx_step;
    float one_over_zy_step;

    float depth_x_step;
    float depth_y_step;
    float light_amount_xstep;
    float light_amount_ystep;

    Gradients(const Vertex& min_y_vert, const Vertex mid_y_vert, const Vertex max_y_vert) {
		float oneOverdX = 1.0f /
			(((mid_y_vert.pos.x - max_y_vert.pos.x) *
			(min_y_vert.pos.y - max_y_vert.pos.y)) -
			((min_y_vert.pos.x - max_y_vert.pos.x) *
			(mid_y_vert.pos.y - max_y_vert.pos.y)));

		float oneOverdY = -oneOverdX;

        depth[0] = min_y_vert.pos.z;
        depth[1] = mid_y_vert.pos.z;
        depth[2] = max_y_vert.pos.z;

        V4F light_dir = V4F(0, 0, -1);
        light_amount[0] = saturate(min_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;
        light_amount[1] = saturate(mid_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;
        light_amount[2] = saturate(max_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;

        // The z value is the occlusion z value.
        one_over_z[0] = 1.0f / min_y_vert.pos.w;
        one_over_z[1] = 1.0f / mid_y_vert.pos.w;
        one_over_z[2] = 1.0f / max_y_vert.pos.w;

        // The w value is the perspective z value.
		text_coords[0] = min_y_vert.text_coords * one_over_z[0];
		text_coords[1] = mid_y_vert.text_coords * one_over_z[1];
		text_coords[2] = max_y_vert.text_coords * one_over_z[2];

        auto calc_x_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.pos.y - max_y_vert.pos.y)) -
                ((f0 - f2) * (mid_y_vert.pos.y - max_y_vert.pos.y))) * oneOverdX;
        };

        auto calc_y_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.pos.x - max_y_vert.pos.x)) -
                ((f0 - f2) * (mid_y_vert.pos.x - max_y_vert.pos.x))) * oneOverdY;
        };

        text_coord_x_xstep = calc_x_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);
        text_coord_x_ystep = calc_y_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);

        text_coord_y_xstep = calc_x_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);
        text_coord_y_ystep = calc_y_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);

        one_over_zx_step = calc_x_step(one_over_z[0], one_over_z[1], one_over_z[2]);
        one_over_zy_step = calc_y_step(one_over_z[0], one_over_z[1], one_over_z[2]);

        depth_x_step = calc_x_step(depth[0], depth[1], depth[2]);
        depth_y_step = calc_y_step(depth[0], depth[1], depth[2]);

        light_amount_xstep = calc_x_step(light_amount[0], light_amount[1], light_amount[2]);
        light_amount_ystep = calc_y_step(light_amount[0], light_amount[1], light_amount[2]);
    }
};

struct Triangle {
    Vertex p[3];
    Triangle() {};
    Triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
        p[0] = v1;
        p[1] = v2;
        p[2] = v3;
    }
};

struct Edge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    float text_coord_x;
    float text_coord_xstep;
    float text_coord_y;
    float text_coord_ystep;

    float one_over_z;
    float one_over_zstep;

    float depth;
    float depth_step;

    float light_amount;
    float light_amount_step;

    Edge() {}
    Edge(const Vertex &min_y_vert, const Vertex &max_y_vert, const Gradients &gradients, int min_y_vert_index) {
    	y_start = (int)std::ceil(min_y_vert.pos.y);
		y_end = (int)std::ceil(max_y_vert.pos.y);

		float y_dist = max_y_vert.pos.y - min_y_vert.pos.y;
		float x_dist = max_y_vert.pos.x- min_y_vert.pos.x;

		float y_prestep = (float) y_start - min_y_vert.pos.y;
		x_step = (float)x_dist/(float)y_dist;
		x = min_y_vert.pos.x + y_prestep * x_step;

        float x_prestep = x - min_y_vert.pos.x;

        text_coord_x = gradients.text_coords[min_y_vert_index].x +
            gradients.text_coord_x_xstep * x_prestep + gradients.text_coord_x_ystep * y_prestep;
        text_coord_xstep = gradients.text_coord_x_ystep + gradients.text_coord_x_xstep * x_step;

        text_coord_y = gradients.text_coords[min_y_vert_index].y +
            gradients.text_coord_y_xstep * x_prestep + gradients.text_coord_y_ystep * y_prestep;
        text_coord_ystep = gradients.text_coord_y_ystep + gradients.text_coord_y_xstep * x_step;

        one_over_z = gradients.one_over_z[min_y_vert_index] +
            gradients.one_over_zx_step * x_prestep +
            gradients.one_over_zy_step * y_prestep;
        one_over_zstep = gradients.one_over_zy_step + gradients.one_over_zx_step * x_step;

        depth = gradients.depth[min_y_vert_index] +
            gradients.depth_x_step * x_prestep +
            gradients.depth_y_step * y_prestep;
        depth_step = gradients.depth_y_step + gradients.depth_x_step * x_step;

        light_amount = gradients.light_amount[min_y_vert_index] +
            gradients.light_amount_xstep * x_prestep +
            gradients.light_amount_ystep * y_prestep;
        light_amount_step = gradients.light_amount_ystep + gradients.light_amount_xstep * x_step;
    }

    constexpr void step() {
        x += x_step;
        text_coord_x += text_coord_xstep;
        text_coord_y += text_coord_ystep;
        one_over_z += one_over_zstep;
        depth += depth_step;
        light_amount += light_amount_step;
    }
};



struct Mesh {
    std::vector<Triangle> triangles;
    Texture *texture;

    Mesh() {}
    bool load_from_obj_file(std::string path);
};

struct Quaternion {
    float x, y, z, w;

    Quaternion() {
        x = y = z = w = 0;
    }

    Quaternion(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Quaternion(V4F axis, float angle) {
        angle = angle /360 * (float)M_PI * 2;

        float sin_half_angle = std::sin(angle / 2.0f);
        float cos_half_angle = std::cos(angle / 2.0f);

        x = axis.x * sin_half_angle;
        y = axis.y * sin_half_angle;
        z = axis.z * sin_half_angle;

        w = cos_half_angle;
    }

    Quaternion(const Matrix4x4 &rot) {
        float trace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];

        if (trace > 0) {
            float s = 0.5f / std::sqrt(trace + 1.0f);
			w = 0.25f / s;
			x = (rot.m[1][2] - rot.m[2][1]) * s;
			y = (rot.m[2][0] - rot.m[0][2]) * s;
			z = (rot.m[0][1] - rot.m[1][0]) * s;
        } else {
            if (rot.m[0][0] > rot.m[1][1] && rot.m[0][0] > rot.m[2][2]) {
                float s = 2.0f * std::sqrt(1.0f + rot.m[0][0] - rot.m[1][1] - rot.m[2][2]);
				w = (rot.m[1][2] - rot.m[2][1] / s);
				x = 0.25f * s;
				y = (rot.m[1][0] + rot.m[0][1]) / s;
				z = (rot.m[2][0] + rot.m[0][2]) / s;
            }
            else if (rot.m[1][1] > rot.m[2][2]) {
                float s = 2.0f * std::sqrt(1.0f + rot.m[1][1] - rot.m[0][0] - rot.m[2][2]);
				w = (rot.m[2][0] - rot.m[0][2]) / s;
				x = (rot.m[1][0] + rot.m[0][1]) / s;
				y = 0.25f * s;
				z = (rot.m[2][1] + rot.m[1][2]) / s;
            }
            else {
                float s = 2.0f * std::sqrt(1.0f + rot.m[2][2] - rot.m[0][0] - rot.m[1][1]);
				w = (rot.m[0][1] - rot.m[1][0]) / s;
				x = (rot.m[2][0] + rot.m[0][2]) / s;
				y = (rot.m[1][2] + rot.m[2][1]) / s;
				z = 0.25f * s;
            }
        }

        float length = std::sqrt(x*x + y*y + z*z + w*w);
        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }

    float length() {
        return std::sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion normalized() {
        float l = length();

        return Quaternion(x / l, y / l, z / l, w / l);
    }

    Quaternion conjugate() const {
        return Quaternion(-x, -y, -z, w);
    }

    float dot(Quaternion r) {
        return x * r.x + y * r.y + z * r.z + w * r.w;
    }

    Matrix4x4 to_rotation_matrix() {
        V4F forward =  V4F(2.0f * (x * z - w * y), 2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + y * y));
		V4F up = V4F(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - w * x));
		V4F right =  V4F(1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - w * z), 2.0f * (x * z + w * y));

        auto r = Math::mat_4x4_rotation(forward.normalized().to_v4d(), up.normalized().to_v4d(), right.normalized().to_v4d());

        return r;
    }

    V4F get_forward() const {
        return V4F(0, 0, 1, 1).rotate(*this);
    }

    V4F get_back() const {
        return V4F(0, 0, -1, 1).rotate(*this);
    }

    V4F get_right() const {
        return V4F(1, 0, 0, 1).rotate(*this);
    }

    V4F get_left() const {
        return V4F(-1, 0, 0, 1).rotate(*this);
    }

    V4F get_up() const {
        return V4F(0, 1, 0, 1).rotate(*this);
    }

    V4F get_down() const {
        return V4F(0, -1, 0, 1).rotate(*this);
    }

    Quaternion operator*(const Quaternion &q) const {
        Quaternion r;
        r.w = w * q.w - x * q.x - y * q.y - z * q.z;
		r.x = x * q.w + w * q.x + y * q.z - z * q.y;
		r.y = y * q.w + w * q.y + z * q.x - x * q.z;
		r.z = z * q.w + w * q.z + x * q.y - y * q.x;

        return r;
    }

    Quaternion operator*(const V4F &v) const {
        Quaternion r;
        r.w = -x * v.x - y * v.y - z * v.z;
        r.x =  w * v.x + y * v.z - z * v.y;
        r.y =  w * v.y + z * v.x - x * v.z;
        r.z =  w * v.z + x * v.y - y * v.x;

        return r;
    }

    Quaternion operator*(float f) const {
        Quaternion r;
        r.x = x * f;
        r.y = y * f;
        r.z = z * f;
        r.w = w * f;

        return r;
    }

    Quaternion operator/(float f) const {
        Quaternion r;
        r.x = x / f;
        r.y = y / f;
        r.z = z / f;
        r.w = w / f;

        return r;
    }
};

struct Transform {
    V4F pos;
    Quaternion rot;
    V4F scale;

    Transform() {
        this->pos = V4F(0, 0, 0, 0);
        this->rot = Quaternion(0, 0, 0, 1);
        this->scale = V4F(1, 1, 1, 1);
    }

    Transform(const V4F &pos) {
        this->pos = pos;
        this->rot = Quaternion(0, 0, 0, 1);
        this->scale = V4F(1, 1, 1, 1);
    }

    Transform(const V4F &pos, const Quaternion &rot, const V4F &scale) {
        this->pos = pos;
        this->rot = rot;
        this->scale = scale;
    }

    void move(const V4F &dir, float amt) {
        set_pos(pos + (dir * amt));
    }

    Matrix4x4 get_matrix_transformation() {
        Matrix4x4 translation, rotation, scale, identity;

        translation = Math::mat_4x4_translation(pos.x,  pos.y, pos.z);
        rotation = rot.conjugate().to_rotation_matrix();
        scale = Math::mat_4x4_scale(this->scale.x, this->scale.y, this->scale.z);

        return rotation * translation * scale;
    }

    void set_pos(const V4F &pos) {
        this->pos = pos;
    }

    Transform& rotate(const Quaternion &rotation) {
        this->rot = (rotation * rot).normalized();

        return *this;
    }

    Transform& look_at(const V4F &point, const V4F &up) {
        Matrix4x4 mat_look_at;
        mat_look_at = Math::mat_4x4_rotation(point.to_v4d(), up.to_v4d());
        this->rot = Quaternion(mat_look_at);

        return *this;
    }
};

V4F vector_intersect_plane(V4F &plane_p, V4F &plane_n, V4F &line_start, V4F &line_end);
int triangle_clip_against_plane(V4F plane_p, V4F plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);
