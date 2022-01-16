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

Vertex Vertex::transform(const Matrix4x4 &m) {
    return Vertex(m.transform(pos), text_coords, normal);
}

void Vertex::normal_transform(const Matrix4x4 &normal_matrix) {
    normal = (normal_matrix.transform(normal)).normalized();
}

Vertex Vertex::transform(const Matrix4x4 &transform, const Matrix4x4 &normal) {
    V4D v = transform.transform(pos);
    V4D n = (normal * this->normal).normalized();
    return Vertex(v, text_coords, n);
}

bool Mesh::load_from_obj_file(std::string path) {
    ObjReader obj_reader;
    if (!obj_reader.read_file(path))
        return false;

    triangles = obj_reader.create_vertices();
    return true;
}

V4F& V4F::rotate(const Quaternion &rot) {
    Quaternion conj = rot.conjugate();
    Quaternion new_rot = rot * *this * conj;

    x = new_rot.x;
    y = new_rot.y;
    z = new_rot.z;
    w = 1.0f;

    return *this;
}
