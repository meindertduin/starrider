#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

namespace Graphics {

    struct Gradients {
    Point2D text_coords[3];

    float text_coord_x_xstep;
    float text_coord_x_ystep;

    float text_coord_y_xstep;
    float text_coord_y_ystep;

    float depth_x_step;
    float depth_y_step;
    float light_amount_xstep;
    float light_amount_ystep;

    Gradients(const Vertex4D& min_y_vert, const Vertex4D mid_y_vert, const Vertex4D max_y_vert) {
		float oneOverdX = 1.0f /
			(((mid_y_vert.v.x - max_y_vert.v.x) *
			(min_y_vert.v.y - max_y_vert.v.y)) -
			((min_y_vert.v.x - max_y_vert.v.x) *
			(mid_y_vert.v.y - max_y_vert.v.y)));

		float oneOverdY = -oneOverdX;

		text_coords[0] = min_y_vert.t;
		text_coords[1] = mid_y_vert.t;
		text_coords[2] = max_y_vert.t;

        auto calc_x_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.v.y - max_y_vert.v.y)) -
                ((f0 - f2) * (mid_y_vert.v.y - max_y_vert.v.y))) * oneOverdX;
        };

        auto calc_y_step = [&](float f0, float f1, float f2) {
            return (((f1 - f2) * (min_y_vert.v.x - max_y_vert.v.x)) -
                ((f0 - f2) * (mid_y_vert.v.x - max_y_vert.v.x))) * oneOverdY;
        };

        text_coord_x_xstep = calc_x_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);
        text_coord_x_ystep = calc_y_step(text_coords[0].x, text_coords[1].x, text_coords[2].x);

        text_coord_y_xstep = calc_x_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);
        text_coord_y_ystep = calc_y_step(text_coords[0].y, text_coords[1].y, text_coords[2].y);
    }
};


struct IGouradEdge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    // lighting
    float di_dy;
    float i;

    IGouradEdge() = default;

    IGouradEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x- min_y_vert.v.x;

        x_step = x_dist / y_dist;
        x = min_y_vert.v.x;

        di_dy = (max_y_vert.i - min_y_vert.i) / (max_y_vert.v.y - min_y_vert.v.y);
        i = min_y_vert.i;
    }
};

struct CGouradEdge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    // lighting
    float dr_dy;
    float dg_dy;
    float db_dy;
    float r;
    float g;
    float b;

    // texturing
    float du_dy;
    float dv_dy;
    float u;
    float v;

    CGouradEdge() = default;

    CGouradEdge(const Vertex4D &min_y_vert, RGBA min_y_vert_col, const Vertex4D &max_y_vert, RGBA max_y_vert_col, const Gradients &gradients, int min_y_vert_index) {
        y_start = min_y_vert.v.y;
        y_end = max_y_vert.v.y;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x- min_y_vert.v.x;

        x_step = x_dist / y_dist;
        x = min_y_vert.v.x;

        // TODO: optiize (max_y_vert.v.y - min_y_vert.v.y)
        dr_dy = (max_y_vert_col.r - min_y_vert_col.r) / (max_y_vert.v.y - min_y_vert.v.y);
        dg_dy = (max_y_vert_col.g - min_y_vert_col.g) / (max_y_vert.v.y - min_y_vert.v.y);
        db_dy = (max_y_vert_col.b - min_y_vert_col.b) / (max_y_vert.v.y - min_y_vert.v.y);

        r = min_y_vert_col.r;
        g = min_y_vert_col.g;
        b = min_y_vert_col.b;

        u = gradients.text_coords[min_y_vert_index].x;
        du_dy = gradients.text_coord_x_ystep + gradients.text_coord_x_xstep * x_step;

        v = gradients.text_coords[min_y_vert_index].y;
        dv_dy = gradients.text_coord_y_ystep + gradients.text_coord_y_xstep * x_step;
    }
};

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void draw_colored_gouraud_triangle(RenderListPoly &poly);

void draw_intensity_gouraud_triangle(RenderListPoly &poly);

void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void scan_edges(IGouradEdge &left, IGouradEdge &right, bool handedness, RGBA color);
void scan_edges(CGouradEdge &left, CGouradEdge &right, bool handedness, RGBA color, const RenderListPoly &poly, Gradients &gradients);

}

