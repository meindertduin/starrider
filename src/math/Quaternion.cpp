#include "Quaternion.h"

namespace Math {
    Quat_Type::Quat_Type(const Quat_Type &other) {
        std::memcpy(m, other.m, sizeof(Quat_Type));
    }

    Quat_Type::Quat_Type(Quat_Type &&other) {
        std::memmove(m, other.m, sizeof(Quat_Type));
    }

    Quat_Type& Quat_Type::operator=(const Quat_Type &other) {
        std::memcpy(m, other.m, sizeof(Quat_Type));
        return *this;
    }

    Quat_Type& Quat_Type::operator=(Quat_Type &&other) {
        std::memmove(m, other.m, sizeof(Quat_Type));
        return *this;
    }
}
