#include "Core.h"
#include <sstream>
#include <fstream>

Vertex Vertex::transform(const Matrix4F &m) {
    return Vertex(m.transform(pos), text_coords);
}

bool Mesh::load_from_obj_file(std::string path) {
    std::ifstream fs(path);

    if (!fs.is_open())
        return false;

    std::vector<Vertex> vertexes;

    while(!fs.eof()) {
        char line[128]; // presumption that lines aren't over 128 characters wide
        fs.getline(line, 128);

        std::stringstream ss;
        ss << line;

        char temp;
        if (line[0] == 'v') {
            float x, y, z;
            ss >> temp >> x >> y >> z;

            vertexes.push_back(Vertex(x, y, z));
        }

        if (line[0] == 'f') {
            int f[3];
            ss >> temp >> f[0] >> f[1] >> f[2];
            triangles.push_back(Triangle(vertexes[f[0] -1], vertexes[f[1] - 1], vertexes[f[2] - 1]));
        }
    }

    fs.close();
    return true;
}

// tests and returns the vector where the line intersects with a plane
Vertex vector_intersect_plane(V4F &plane_p, V4F &plane_n, Vertex &line_start, Vertex &line_end) {
    plane_n.normalise();
    float plane_d = -plane_n.prod(plane_p);
    float ad = line_start.pos.prod(plane_n);
    float bd = line_end.pos.prod(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vertex lineStartToEnd = line_end - line_start;
    Vertex lineToIntersect = lineStartToEnd * t;
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
        out_tri1.p[1] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1;
    }

    if (inside_points_count == 2 && outside_points_count == 1)
    {
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];
        out_tri1.p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2;
    }

    return 0;
}
