#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

namespace Graphics {

struct Edge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    Edge() = default;

    Edge(float min_y_x, float min_y_y, float max_y_x, float max_y_y) {
        y_start = min_y_y;
        y_end = max_y_y;

        float y_dist = max_y_y - min_y_y;
        float x_dist = max_y_x- min_y_x;

        x_step = x_dist / y_dist;
        x = min_y_x;

        float x_prestep = x - min_y_x;
    }
};

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void scan_edges(Edge &left, Edge &right, bool handedness, uint32_t color);

}

