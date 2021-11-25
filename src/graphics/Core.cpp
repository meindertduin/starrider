#include "Core.h"

Vertex Vertex::transform(const Matrix4F &m) {
    return Vertex(m.transform(pos), text_coords);
}
