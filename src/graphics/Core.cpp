#include "Core.h"

V3F V3F::transform(const Matrix4F &m) {
    return m.transform(*this);
}
