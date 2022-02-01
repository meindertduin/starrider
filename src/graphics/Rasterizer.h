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
    	y_start = min_y_vert.pos.y + 1.0f;
		y_end = max_y_vert.pos.y + 1.0f;

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

    constexpr void step() {
        x += x_step;
    }
};

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void draw_triangle(V4D points[3], RGBA color);
    void clear_depth_buffer();
    void set_viewport(int width, int height);
private:
    int m_width, m_height;
    Renderer* p_renderer = nullptr;
    float *p_z_buffer = nullptr;

    Pixel* p_framebuffer;

    inline void scan_edges(Edge &a, Edge &b, bool handedness, RGBA color);
    void draw_scanline(const Edge &left, const Edge &right, int y, RGBA color) {
        // fill convention ceils up
        int x_min = left.x + 1.0f;
        int x_max = right.x + 1.0f;

        for(int x = x_min; x < x_max; x++)
        {
            if (x > 0 && y > 0 && x < m_width && y < m_height)
                p_framebuffer[m_width * y + x].value = rgba_bit(color.r, color.g, color.b, color.a);
        }
    }
};
