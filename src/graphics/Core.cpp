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
