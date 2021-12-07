#include "Core.h"
#include <sstream>
#include <fstream>

#include "../io/ObjReader.h"

float saturate(float val) {
    if (val < 0.0f) {
        return 0.0f;
    }
    if (val > 1.0f) {
        return 1.0f;
    }

    return val;
}

Vertex Vertex::transform(const Matrix4F &m) {
    return Vertex(m.transform(pos), text_coords, normal);
}

bool Mesh::load_from_obj_file(std::string path) {
    ObjReader obj_reader;
    return obj_reader.read_file(path);
}

// tests and returns the vector where the line intersects with a plane
V4F vector_intersect_plane(V4F &plane_p, V4F &plane_n, V4F &line_start, V4F &line_end) {
    plane_n.normalise();
    float plane_d = -plane_n.prod(plane_p);
    float ad = line_start.prod(plane_n);
    float bd = line_end.prod(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    V4F lineStartToEnd = line_end - line_start;
    V4F lineToIntersect = lineStartToEnd * t;
    return line_start + lineToIntersect;
}

// returns the amount of triangles that clip against a plane
int triangle_clip_against_plane(V4F plane_p, V4F plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2) {
    plane_n.normalise();

    auto dist = [&](V4F &p)
    {
        V4F n = p;
        n.normalise();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.prod(plane_p));
    };

    Vertex* inside_points[3];  int inside_points_count = 0;
    Vertex* outside_points[3]; int outside_points_count = 0;

    float d0 = dist(in_tri.p[0].pos);
    float d1 = dist(in_tri.p[1].pos);
    float d2 = dist(in_tri.p[2].pos);

    if (d0 >= 0) {
        inside_points[inside_points_count++] = &in_tri.p[0];
    } else {
        outside_points[outside_points_count++] = &in_tri.p[0];
    }
    if (d1 >= 0) {
        inside_points[inside_points_count++] = &in_tri.p[1];
    } else {
        outside_points[outside_points_count++] = &in_tri.p[1];
    }

    if (d2 >= 0) {
        inside_points[inside_points_count++] = &in_tri.p[2];
    }
    else {
        outside_points[outside_points_count++] = &in_tri.p[2];
    }

    if (inside_points_count == 0) {
        return 0;
    }

    if (inside_points_count == 3) {
        out_tri1 = in_tri;

        return 1;
    }

    if (inside_points_count == 1 && outside_points_count == 2)
    {
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[0].text_coords = inside_points[0]->text_coords;

        out_tri1.p[1].pos = vector_intersect_plane(plane_p, plane_n, inside_points[0]->pos, outside_points[0]->pos);
        out_tri1.p[1].text_coords = outside_points[0]->text_coords;

        out_tri1.p[2].pos = vector_intersect_plane(plane_p, plane_n, inside_points[0]->pos, outside_points[1]->pos);
        out_tri1.p[2].text_coords = outside_points[1]->text_coords;

        return 1;
    }

    if (inside_points_count == 2 && outside_points_count == 1)
    {
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];
        out_tri1.p[2].pos = vector_intersect_plane(plane_p, plane_n, inside_points[0]->pos, outside_points[0]->pos);
        out_tri1.p[2].text_coords = outside_points[0]->text_coords;

        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2].pos = vector_intersect_plane(plane_p, plane_n, inside_points[1]->pos, outside_points[0]->pos);
        out_tri2.p[2].text_coords = inside_points[0]->text_coords;

        return 2;
    }

    return 0;
}
