#pragma once

#include "Core.h"
#include "Renderer.h"

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    void draw_triangle(const Triangle &triangle, const Bitmap &texture);
private:
    Renderer* p_renderer;

    void scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Bitmap &texture);
    void scan_edges(Edge &a, Edge &b, bool handedness, const Bitmap &texture);
    void draw_scanline(const Edge &left, const Edge &right, int j, const Bitmap &texture);
};
