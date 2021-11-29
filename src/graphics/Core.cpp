#include "Core.h"

Vertex Vertex::transform(const Matrix4F &m) {
    return Vertex(m.transform(pos), text_coords);
}

Triangle::Triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
    p[0] = v1;
    p[1] = v2;
    p[2] = v3;
}

Vertex::Vertex() {}

Vertex::Vertex(float x, float y, float z) {
    pos = V4F(x, y, z);
}
