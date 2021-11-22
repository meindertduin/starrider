#pragma once

struct Point {
    int x, y;
};

struct V3D {
    int x, y, z;
};

struct Triangle {
    V3D p[3];
};

struct Edge {
    Point p[2];
    Edge() {}
    Edge(const V3D &p1, const V3D &p2) {
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
