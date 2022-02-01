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

    Edge() {}
    Edge(const Vertex &min_y_vert, const Vertex &max_y_vert) {
        // fill convention is to ceil up
    	y_start = std::ceil(min_y_vert.pos.y);
		y_end = std::ceil(max_y_vert.pos.y) - 1;

		float y_dist = max_y_vert.pos.y - min_y_vert.pos.y;
		float x_dist = max_y_vert.pos.x- min_y_vert.pos.x;

		x_step = x_dist / y_dist;
		x = min_y_vert.pos.x;

        float x_prestep = x - min_y_vert.pos.x;
    }

    Edge(const V4D &min_y_vert, const V4D &max_y_vert) {
    	y_start = min_y_vert.y;
		y_end = max_y_vert.y;

		float y_dist = max_y_vert.y - min_y_vert.y;
		float x_dist = max_y_vert.x- min_y_vert.x;

		x_step = x_dist / y_dist;
		x = min_y_vert.x;

        float x_prestep = x - min_y_vert.x;
    }

    Edge(float min_y_x, float min_y_y, float max_y_x, float max_y_y) {
    	y_start = min_y_y;
		y_end = max_y_y;

		float y_dist = max_y_y - min_y_y;
		float x_dist = max_y_x- min_y_x;

		x_step = x_dist / y_dist;
		x = min_y_x;

        float x_prestep = x - min_y_x;
    }

    constexpr void step() {
        x += x_step;
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

    inline void scan_edges(Edge &a, Edge &b, bool handedness, uint32_t color);
};
