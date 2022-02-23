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

