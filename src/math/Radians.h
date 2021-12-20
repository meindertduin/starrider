#pragma once

#include <math.h>

float deg_to_half_rad(float degrees) {
    return 1.0 / std::tan(degrees * 0.5f / 180.0f * 3.14159f);
}
