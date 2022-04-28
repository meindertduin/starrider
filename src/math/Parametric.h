#pragma once

#include "Vector.h"

namespace Math {
static constexpr int PAR_LINE_NO_INTERSECT = 0;
static constexpr int PAR_LINE_INTERSECT_IN_SEGMENT = 1;
static constexpr int PAR_LINE_INTERSECT_OUT_SEGMENT = 2;
static constexpr int PAR_LINE_INTERSECT_EVERYWHERE = 3;

typedef struct ParLine2D_Type {
    Point2D p0; // start point
    Point2D p1; // end point
    V2D v; // direction of vector

    ParLine2D_Type() = default;
    constexpr ParLine2D_Type(const Point2D &p_init, const Point2D &p_term)
        : p0(p_init), p1(p_term), v(p_term.x - p_init.x, p_term.y - p_init.y) {}

    Point2D compute_point(float t) const {
        return p0 + v * t;
    }

    /*
     * Returns 0 if no intersection takes place.
     * Returns 1 when intersection takes place within segment [0, 1]
     * Returns 2 when intersection takes place outside segment [0, 1]
     */
    int get_intersect(ParLine2D_Type &other, float &t, float &t_other) const;
    int get_intersect(ParLine2D_Type &other, Point2D &p_intersect) const;

} ParLine2D, *ParLine2D_Ptr;

typedef struct ParLine3D_Type {
    Point3D p0; // start point
    Point3D p1; // end point
    V3D v; // direction of vector

    ParLine3D_Type() = default;
    constexpr ParLine3D_Type(const Point3D &p_init, const Point3D &p_term)
        : p0(p_init), p1(p_term), v(p_term.x - p_init.x, p_term.y - p_init.y, p_term.z - p_init.z) {}

    Point3D compute_point(float t) const {
        return p0 + v * t;
    }
} ParLine3D, *ParLine3D_Ptr;

typedef struct Plane3D_Type {
    Point3D p0; // point of plane
    V3D n; // normal of the plane

    Plane3D_Type() = default;
    Plane3D_Type(const Point3D &p0, V3D &normal, int normalise)
        : p0(p0), n(normalise ? normal.normalized() : normal) {}

    float point_in_plane(const Point3D &p) const;
    ParLine3D parm_line(const Point3D &p, float t) const;

    /*
     * Returns 0 if point lies within plane
     * Returns < 0 if point lies in negative half space
     * Returns > 0 if point lies in positive half space
     * */
    constexpr float compute_point(const Point3D &point) const {
        return n.x * (point.x - p0.x) + n.y * (point.y - p0.y) + n.z * (point.z - p0.z);
    }

    /*
     * Returns 0 if no intersection takes place.
     * Returns 1 when intersection takes place within segment [0, 1]
     * Returns 2 when intersection takes place outside segment [0, 1]
     * Returns 2 when intersection takes everywhere (most likely due to line and plain being parallel)
     * */
    int get_intersect(ParLine3D &line, float &t, Point3D &p_intersect) const;

} Plane3D, *Plane3D_Ptr;

typedef struct Box_Type {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    float min_z;
    float max_z;

    Box_Type() = default;
    Box_Type(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z);

    /*
     * Returns 0 if the point lies exactly on the box
     * Returns -1 if the point lies outside the plane
     * Returns 1 if the point lies within the box
     * */
    int point_in_box(const Point3D &p) const;
} Box, *Box_Ptr;
}
