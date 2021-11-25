#pragma once

#include <math.h>
#include <stdint.h>

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
        uint32_t rr = (uint32_t)(r * 255.0f + 0.5f);
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

struct Matrix4F;

struct V3F {
    float x, y, z, w;
    Color color;
    V3F() {
        x = y = z = 0;
        w = 1;
    }

    V3F(const float &in_x, const float &in_y, const float &in_z) {
        x = in_x;
        y = in_y;
        z = in_z;
        w = 1;
    }

    V3F(const float &in_x, const float &in_y, const float &in_z, const float &in_w) {
        x = in_x;
        y = in_y;
        z = in_z;
        w = in_w;
    }

    V3F transform(const Matrix4F &m);

    V3F& perspective_divide() {
        if (w != 0.0) {
            x /= w; y /= w; z/= w;
        }

        return *this;
    }
};

struct Gradients {
    Color color[3];
    Color x_step;
    Color y_step;

    Gradients(const V3F& min_y_vert, const V3F mid_y_vert, const V3F max_y_vert) {
		color[0] = min_y_vert.color;
		color[1] = mid_y_vert.color;
		color[2] = max_y_vert.color;

		float oneOverdX = 1.0f /
			(((mid_y_vert.x - max_y_vert.x) *
			(min_y_vert.y - max_y_vert.y)) -
			((min_y_vert.x - max_y_vert.x) *
			(mid_y_vert.y - max_y_vert.y)));

		float oneOverdY = -oneOverdX;

        x_step = (((color[1] - color[2]) * (min_y_vert.y - max_y_vert.y)) -
             ((color[0] - color[2]) * (mid_y_vert.y - max_y_vert.y))) * oneOverdX;

        y_step = (((color[1] - color[2]) * (min_y_vert.x - max_y_vert.x)) -
                ((color[0] - color[2]) * (mid_y_vert.x - max_y_vert.x))) * oneOverdY;
    }
};

struct Matrix4F {
    float m[4][4];

    void init_identity() {
        m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
    }

    void init_screen_space_transform(float half_width, float half_height) {
        m[0][0] = half_width;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = half_width;
		m[1][0] = 0;	m[1][1] = -half_height;	m[1][2] = 0;	m[1][3] = half_height;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
    }

    void init_translation(float x, float y, float z) {
        m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
    }

    void init_rotation_x(float x) {
        m[0][0] = 1;	m[0][1] = 0;			m[0][2] = 0;				m[0][3] = 0;
        m[1][0] = 0;	m[1][1] = std::cos(x);  m[1][2] = std::sin(x);    m[1][3] = 0;
        m[2][0] = 0;	m[2][1] = -std::sin(x);  m[2][2] = std::cos(x);      m[2][3] = 0;
        m[3][0] = 0;	m[3][1] = 0;			m[3][2] = 0;				m[3][3] = 1;
    }

    void init_rotation_y(float y) {
        m[0][0] = std::cos(y);      m[0][1] = 0;	m[0][2] = - std::sin(y);    m[0][3] = 0;
		m[1][0] = 0;			    m[1][1] = 1;	m[1][2] = 0;				m[1][3] = 0;
		m[2][0] = std::sin(y);      m[2][1] = 0;	m[2][2] = std::cos(y);      m[2][3] = 0;
		m[3][0] = 0;			    m[3][1] = 0;	m[3][2] = 0;			    m[3][3] = 1;
    }

    void init_rotation_z(float z) {
        m[0][0] = std::cos(z);  m[0][1] = - std::sin(z);    m[0][2] = 0;	m[0][3] = 0;
		m[1][0] = std::sin(z);  m[1][1] = std::cos(z);      m[1][2] = 0;	m[1][3] = 0;
		m[2][0] = 0;			m[2][1] = 0;				m[2][2] = 1;	m[2][3] = 0;
		m[3][0] = 0;			m[3][1] = 0;				m[3][2] = 0;	m[3][3] = 1;
    }

    void init_rotation(float x, float y, float z) {
        Matrix4F rot_x, rot_y, rot_z;

        rot_x.init_rotation_x(x);
        rot_y.init_rotation_y(y);
        rot_z.init_rotation_z(z);

        auto matrix = (rot_x * rot_y * rot_z);

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = matrix.m[i][j];
    }

    void init_perspective(float fov, float aspect_ratio, float znear, float zfar) {
        float zrange = znear - zfar;

        m[0][0] = fov * aspect_ratio;	            m[0][1] = 0.0f;	            m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;                             m[1][1] = 1.0f / fov;	m[1][2] = 0;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	                            m[2][1] = 0.0f;				m[2][2] = (-znear -zfar)/zrange;	m[2][3] = zfar * znear / zrange;
		m[3][0] = 0.0f;	                            m[3][1] = 0.0f;				m[3][2] = 1.0f;	m[3][3] = 0.0f;

    }

    V3F transform(const V3F &r) const {
        return V3F(m[0][0] * r.x + m[0][1] * r.y + m[0][2] * r.z + m[0][3] * r.z,
            m[1][0] * r.x + m[1][1] * r.y + m[1][2] * r.z + m[1][3] * r.w,
            m[2][0] * r.x + m[2][1] * r.y + m[2][2] * r.z + m[2][3] * r.w,
            m[3][0] * r.x + m[3][1] * r.y + m[3][2] * r.z + m[3][3] * r.w);
    }

    Matrix4F operator*(const Matrix4F &m1) {
        Matrix4F matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m1.m[r][0] * m[0][c] + m1.m[r][1] * m[1][c] + m1.m[r][2] * m[2][c] + m1.m[r][3] * m[3][c];
		return matrix;
    }
};

struct Triangle {
    V3F p[3];
};

struct Edge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    Color color;
    Color color_step;

    Edge() {}
    Edge(const V3F &min_y_vert, const V3F &max_y_vert, const Gradients &gradients, int min_y_vert_index) {
    	y_start = (int)std::ceil(min_y_vert.y);
		y_end = (int)std::ceil(max_y_vert.y);

		float y_dist = max_y_vert.y - min_y_vert.y;
		float x_dist = max_y_vert.x- min_y_vert.x;

		float y_prestep = (float) y_start - min_y_vert.y;
		x_step = (float)x_dist/(float)y_dist;
		x = min_y_vert.x + y_prestep * x_step;

        float x_prestep = x - min_y_vert.x;

        color = gradients.color[min_y_vert_index] + (gradients.y_step * y_prestep) + (gradients.x_step * x_prestep);
        color_step = gradients.y_step + gradients.x_step * x_step;
    }

    void step() {
        x += x_step;
        color += color_step;
    }
};
