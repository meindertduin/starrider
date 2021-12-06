#pragma once

#include "Core.h"
#include "Renderer.h"

class Rasterizer {
public:
    Rasterizer(Renderer* renderer, int width, int height);
    ~Rasterizer();
    void draw_triangle(const Triangle &triangle, const Bitmap &texture);
    void clear_depth_buffer();
private:
    Renderer* p_renderer;
    int m_width, m_height;
    float *p_z_buffer;

    void scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Bitmap &texture);
    void scan_edges(Edge &a, Edge &b, bool handedness, const Bitmap &texture, const Gradients &gradients);
    void draw_scanline(const Edge &left, const Edge &right, int j, const Bitmap &texture, const Gradients &gradients);
};
