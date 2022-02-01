#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

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

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void draw_triangle(V4D points[3], RGBA color);
    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);
    void set_viewport(int width, int height);
private:
    int min_clip_y {0};
    int min_clip_x {0};
    int m_width, m_height;

    Renderer* p_renderer = nullptr;
    Pixel* p_framebuffer;

    void scan_edges(Edge &left, Edge &right, bool handedness, uint32_t color);
};
