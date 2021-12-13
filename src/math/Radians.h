#pragma once

#include <math.h>

float deg_to_rad(float degrees) {
    return 1.0 / std::tan(90.0f / 180.0f * 3.14159f);
}
