#include "Parametric.h"
#include "Core.h"

#include <math.h>

namespace Math {
    int ParLine2D::get_intersect(ParLine2D_Type &other, float &t, float &t_other) const {
        float det = v.x * other.v.y - v.y * other.v.x;
        if (std::fabs(det) <= EPSILON_E5) {
            return PAR_LINE_NO_INTERSECT;
        }

        t = (other.v.x * (p0.y - other.p0.y) - other.v.y * (p0.x - other.p0.x)) / det;
        t_other = (v.x * (p0.y - other.p0.y) - v.y * (p0.x - other.p0.x)) / det;

        if ((t >= 0) && (t_other <= 1) && (t_other >= 0) && (t_other <= 1))
            return PAR_LINE_INTERSECT_IN_SEGMENT;

        return PAR_LINE_INTERSECT_OUT_SEGMENT;
    }

    int ParLine2D::get_intersect(ParLine2D_Type &other, Point2D &p_intersect) const {
        float t, t_other;
        auto intersect_result = get_intersect(other, t, t_other);

        if (intersect_result) {
            p_intersect = compute_point(t);
            return intersect_result;
        } else {
            return intersect_result;
        }
    }

    int Plane3D_Type::get_intersect(ParLine3D &line, float &t, Point3D &p_intersect) const {
        float plane_dot = line.v.dot(n);

        if (std::fabs(plane_dot) <= EPSILON_E5) {
            if (std::fabs(compute_point(line.p0))) {
                return PAR_LINE_INTERSECT_EVERYWHERE;
            } else {
                return PAR_LINE_NO_INTERSECT;
            }
        }

        t = - (n.x * line.p0.x + n.y * line.p0.y + n.z * line.p0.z -
                n.x * p0.x - n.y * p0.y - n.z * p0.z) / plane_dot;

        p_intersect.x = line.p0.x + line.v.x * t;
        p_intersect.y = line.p0.y + line.v.y * t;
        p_intersect.z = line.p0.z + line.v.z * t;

        if (t >= 0.0f && t <= 1.0f) {
            return PAR_LINE_INTERSECT_IN_SEGMENT;
        } else {
            return PAR_LINE_INTERSECT_OUT_SEGMENT;
        }
    }
}
