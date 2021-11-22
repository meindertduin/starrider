#pragma once

#include <math.h>

struct Point {
    int x, y;
};

struct Matrix4F;

struct V3F {
    float x, y, z, w;
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

    void init_translation(const float &x, const float &y, const float &z) {
        m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
    }

    void init_rotation(float x, float y, float z, float angle) {
        float sin = std::sin(angle);
        float cos = std::cos(angle);

        m[0][0] = cos+x*x*(1-cos);      m[0][1] = x*y*(1-cos)-z*sin;    m[0][2] = x*z*(1-cos)+y*sin;    m[0][3] = 0;
		m[1][0] = y*x*(1-cos)+z*sin;    m[1][1] = cos+y*y*(1-cos);	    m[1][2] = y*z*(1-cos)-x*sin;    m[1][3] = 0;
		m[2][0] = z*x*(1-cos)-y*sin;    m[2][1] = z*y*(1-cos)+x*sin;    m[2][2] = cos+z*z*(1-cos);      m[2][3] = 0;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
    }

    void init_rotation_x(float x) {
        m[0][0] = 1;	m[0][1] = 0;			m[0][2] = 0;				m[0][3] = 0;
        m[1][0] = 0;	m[1][1] = std::cos(x);  m[1][2] = - std::sin(x);    m[1][3] = 0;
        m[2][0] = 0;	m[2][1] = std::sin(x);  m[2][2] = std::cos(x);      m[2][3] = 0;
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

    V3F transform(const V3F &r) const {
        return V3F(m[0][0] * r.x + m[0][1] * r.y + m[0][2] * r.z + m[0][3] * r.z,
		                    m[1][0] * r.x + m[1][1] * r.y + m[1][2] * r.z + m[1][3] * r.w,
		                    m[2][0] * r.x + m[2][1] * r.y + m[2][2] * r.z + m[2][3] * r.w,
							m[3][0] * r.x + m[3][1] * r.y + m[3][2] * r.z + m[3][3] * r.w);
    }
};

struct Triangle {
    V3F p[3];
};

struct Edge {
    Point p[2];
    Edge() {}
    Edge(const V3F &p1, const V3F &p2) {
        if (p1.y < p2.y) {
            p[0].x = p1.x;
            p[0].y = p1.y;

            p[1].x = p2.x;
            p[1].y = p2.y;
        } else {
            p[0].x = p2.x;
            p[0].y = p2.y;

            p[1].x = p1.x;
            p[1].y = p1.y;
        }
    }
};

struct Span {
    int x1, x2;

    Span(const int &in_x1, const int &in_x2) {
        if (in_x1 < in_x2) {
            x1 = in_x1;
            x2 = in_x2;
        } else {
            x1 = in_x2;
            x2 = in_x1;
        }
    }
};
