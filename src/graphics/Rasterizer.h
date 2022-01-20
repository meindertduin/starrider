#pragma once

#include "Core.h"
#include "Renderer.h"
#include "Texture.h"


struct Gradients {
    V4D text_coords[3];
    float one_over_z[3];
    float depth[3];
    float light_amount[3];

    float text_coord_x_xstep;
    float text_coord_x_ystep;

    float text_coord_y_xstep;
    float text_coord_y_ystep;

    float one_over_zx_step;
    float one_over_zy_step;

    float depth_x_step;
    float depth_y_step;
    float light_amount_xstep;
    float light_amount_ystep;

    Gradients(const Vertex& min_y_vert, const Vertex mid_y_vert, const Vertex max_y_vert) {
		float oneOverdX = 1.0f /
			(((mid_y_vert.pos.x - max_y_vert.pos.x) *
			(min_y_vert.pos.y - max_y_vert.pos.y)) -
			((min_y_vert.pos.x - max_y_vert.pos.x) *
			(mid_y_vert.pos.y - max_y_vert.pos.y)));

		float oneOverdY = -oneOverdX;

        depth[0] = min_y_vert.pos.z;
        depth[1] = mid_y_vert.pos.z;
        depth[2] = max_y_vert.pos.z;

        V4D light_dir = V4D(0, 0, -1);
        light_amount[0] = saturate(min_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;
        light_amount[1] = saturate(mid_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;
        light_amount[2] = saturate(max_y_vert.normal.dot(light_dir)) * 0.9f + 0.1f;

        // The z value is the occlusion z value.
        one_over_z[0] = 1.0f / min_y_vert.pos.w;
        one_over_z[1] = 1.0f / mid_y_vert.pos.w;
        one_over_z[2] = 1.0f / max_y_vert.pos.w;

        // The w value is the perspective z value.
		text_coords[0] = min_y_vert.text_coords * one_over_z[0];
		text_coords[1] = mid_y_vert.text_coords * one_over_z[1];
		text_coords[2] = max_y_vert.text_coords * one_over_z[2];

        auto calc_x_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.pos.y - max_y_vert.pos.y)) -
                ((f0 - f2) * (mid_y_vert.pos.y - max_y_vert.pos.y))) * oneOverdX;
        };

        auto calc_y_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.pos.x - max_y_vert.pos.x)) -
                ((f0 - f2) * (mid_y_vert.pos.x - max_y_vert.pos.x))) * oneOverdY;
        };

        text_coord_x_xstep = calc_x_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);
        text_coord_x_ystep = calc_y_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);

        text_coord_y_xstep = calc_x_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);
        text_coord_y_ystep = calc_y_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);

        one_over_zx_step = calc_x_step(one_over_z[0], one_over_z[1], one_over_z[2]);
        one_over_zy_step = calc_y_step(one_over_z[0], one_over_z[1], one_over_z[2]);

        depth_x_step = calc_x_step(depth[0], depth[1], depth[2]);
        depth_y_step = calc_y_step(depth[0], depth[1], depth[2]);

        light_amount_xstep = calc_x_step(light_amount[0], light_amount[1], light_amount[2]);
        light_amount_ystep = calc_y_step(light_amount[0], light_amount[1], light_amount[2]);
    }
};

struct Edge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    float text_coord_x;
    float text_coord_xstep;
    float text_coord_y;
    float text_coord_ystep;

    float one_over_z;
    float one_over_zstep;

    float depth;
    float depth_step;

    float light_amount;
    float light_amount_step;

    Edge() {}
    Edge(const Vertex &min_y_vert, const Vertex &max_y_vert, const Gradients &gradients, int min_y_vert_index) {
    	y_start = (int)std::ceil(min_y_vert.pos.y);
		y_end = (int)std::ceil(max_y_vert.pos.y);

		float y_dist = max_y_vert.pos.y - min_y_vert.pos.y;
		float x_dist = max_y_vert.pos.x- min_y_vert.pos.x;

		float y_prestep = (float) y_start - min_y_vert.pos.y;
		x_step = (float)x_dist/(float)y_dist;
		x = min_y_vert.pos.x + y_prestep * x_step;

        float x_prestep = x - min_y_vert.pos.x;

        text_coord_x = gradients.text_coords[min_y_vert_index].x +
            gradients.text_coord_x_xstep * x_prestep + gradients.text_coord_x_ystep * y_prestep;
        text_coord_xstep = gradients.text_coord_x_ystep + gradients.text_coord_x_xstep * x_step;

        text_coord_y = gradients.text_coords[min_y_vert_index].y +
            gradients.text_coord_y_xstep * x_prestep + gradients.text_coord_y_ystep * y_prestep;
        text_coord_ystep = gradients.text_coord_y_ystep + gradients.text_coord_y_xstep * x_step;

        one_over_z = gradients.one_over_z[min_y_vert_index] +
            gradients.one_over_zx_step * x_prestep +
            gradients.one_over_zy_step * y_prestep;
        one_over_zstep = gradients.one_over_zy_step + gradients.one_over_zx_step * x_step;

        depth = gradients.depth[min_y_vert_index] +
            gradients.depth_x_step * x_prestep +
            gradients.depth_y_step * y_prestep;
        depth_step = gradients.depth_y_step + gradients.depth_x_step * x_step;

        light_amount = gradients.light_amount[min_y_vert_index] +
            gradients.light_amount_xstep * x_prestep +
            gradients.light_amount_ystep * y_prestep;
        light_amount_step = gradients.light_amount_ystep + gradients.light_amount_xstep * x_step;
    }

    constexpr void step() {
        x += x_step;
        text_coord_x += text_coord_xstep;
        text_coord_y += text_coord_ystep;
        one_over_z += one_over_zstep;
        depth += depth_step;
        light_amount += light_amount_step;
    }
};

class Rasterizer {
public:
    Rasterizer(Renderer* renderer);
    ~Rasterizer();
    void draw_triangle(Triangle &triangle, const Texture &texture);
    void fill_triangle(Triangle &triangle, const Texture &texture);
    void clear_depth_buffer();
    void set_viewport(int width, int height);
private:
    int m_width, m_height;
    Renderer* p_renderer = nullptr;
    float *p_z_buffer = nullptr;

    Pixel* p_framebuffer;

    inline void scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Texture &texture);
    inline void scan_edges(Edge &a, Edge &b, bool handedness, const Texture &texture, const Gradients &gradients);
    inline void draw_scanline(const Edge &left, const Edge &right, int j, const Texture &texture, const Gradients &gradients);
    inline void clip_polygon_component(std::vector<Vertex> &vertices, int component_index, float component_factor, std::vector<Vertex> &result);
    inline bool clip_polygon_axis(std::vector<Vertex> &vertices, std::vector<Vertex> &auxilary_list, int component_index);
};
