#pragma once

#include "Core.h"
#include "Renderer.h"

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void draw_triangle(Triangle &triangle, const Bitmap &texture);
    void fill_triangle(Triangle &triangle, const Bitmap &texture);
    void clear_depth_buffer();
    void set_viewport(int width, int height);
private:
    int m_width, m_height;
    Renderer* p_renderer = nullptr;
    float *p_z_buffer = nullptr;

    void scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Bitmap &texture);
    void scan_edges(Edge &a, Edge &b, bool handedness, const Bitmap &texture, const Gradients &gradients);
    void draw_scanline(const Edge &left, const Edge &right, int j, const Bitmap &texture, const Gradients &gradients);
    void clip_polygon_component(std::vector<Vertex> &vertices, int component_index, float component_factor, std::vector<Vertex> &result);
    bool clip_polygon_axis(std::vector<Vertex> &vertices, std::vector<Vertex> &auxilary_list, int component_index);
};
