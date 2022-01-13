#include <math.h>

#include "Core.h"

namespace Math {

float fast_sin(float theta) {
    theta = fmodf(theta, 360);

    // make theta pisitive
    if (theta < 0) theta += 360.0f;

    auto theta_int = static_cast<int>(theta);
    float theta_frac = theta - theta_int;

    return (sin_lookup[theta_int] + theta_frac * (sin_lookup[theta_int + 1] - sin_lookup[theta_int]));
}

float fast_cos(float theta) {
    theta = fmodf(theta, 360);

    // make theta positive
    if (theta < 0) theta += 360.0f;

    auto theta_int = static_cast<int>(theta);
    float theta_frac = theta - theta_int;

    return (cos_lookup[theta_int] + theta_frac * (cos_lookup[theta_int + 1] - cos_lookup[theta_int]));
}

void build_lookup_tables() {
    sin_lookup[360] = 0;
    cos_lookup[360] = 0;

    for (int i = 0; i < 360; ++i) {
        sin_lookup[i] = std::sin(i);
        cos_lookup[i] = std::cos(i);
    }

}

}

