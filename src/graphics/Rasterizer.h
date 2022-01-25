#pragma once

#include "Core.h"
#include "Renderer.h"
#include "Texture.h"

struct Edge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    Edge() {}
    Edge(const Vertex &min_y_vert, const Vertex &max_y_vert) {
    	y_start = min_y_vert.pos.y;
		y_end = max_y_vert.pos.y;

		float y_dist = max_y_vert.pos.y - min_y_vert.pos.y;
		float x_dist = max_y_vert.pos.x- min_y_vert.pos.x;

		x_step = x_dist / y_dist;
		x = min_y_vert.pos.x;

        float x_prestep = x - min_y_vert.pos.x;
    }

    constexpr void step() {
        x += x_step;
    }
};

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void draw_triangle(Triangle &triangle, const Texture &texture);
    void clear_depth_buffer();
    void set_viewport(int width, int height);
private:
    int m_width, m_height;
    Renderer* p_renderer = nullptr;
    float *p_z_buffer = nullptr;

    Pixel* p_framebuffer;

    inline void scan_edges(Edge &a, Edge &b, bool handedness, const Texture &texture);
    void draw_scanline(const Edge &left, const Edge &right, int y, const Texture &texture) {
        int x_min = left.x;
        int x_max = right.x;

        for(int x = x_min; x < x_max; x++)
        {
            if (x > 0 && y > 0 && x < m_width && y < m_height)
                p_framebuffer[m_width * y + x].value = 0x00ff0000;
        }
    }
};
