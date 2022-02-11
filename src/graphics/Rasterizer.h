#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

namespace Graphics {

struct IGouradEdge {
    float x;
    float x_step;
    int y_start;
    int y_end;

    // lighting
    float di_dy;
    float i;

    // texturing
    float du_dy;
    float dv_dy;
    float u;
    float v;

    IGouradEdge() = default;

    IGouradEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        x_step = x_dist / y_dist;
        x = min_y_vert.v.x;

        di_dy = (max_y_vert.i - min_y_vert.i) / y_dist;
        i = min_y_vert.i;

        u = min_y_vert.t.x;
        v = min_y_vert.t.y;

        du_dy = ((max_y_vert.t.x - min_y_vert.t.x) / y_dist);
        dv_dy = ((max_y_vert.t.y - min_y_vert.t.y) / y_dist);
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

    CGouradEdge(const Vertex4D &min_y_vert, RGBA min_y_vert_col, const Vertex4D &max_y_vert, RGBA max_y_vert_col) {
        y_start = min_y_vert.v.y;
        y_end = max_y_vert.v.y;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x- min_y_vert.v.x;

        x_step = x_dist / y_dist;
        x = min_y_vert.v.x;

        dr_dy = (max_y_vert_col.r - min_y_vert_col.r) / y_dist;
        dg_dy = (max_y_vert_col.g - min_y_vert_col.g) / y_dist;
        db_dy = (max_y_vert_col.b - min_y_vert_col.b) / y_dist;

        r = min_y_vert_col.r;
        g = min_y_vert_col.g;
        b = min_y_vert_col.b;

        u = min_y_vert.t.x;
        v = min_y_vert.t.y;

        du_dy = ((max_y_vert.t.x - min_y_vert.t.x) / y_dist);
        dv_dy = ((max_y_vert.t.y - min_y_vert.t.y) / y_dist);
    }
};

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void draw_colored_gouraud_triangle(RenderListPoly &poly);

void draw_intensity_gouraud_triangle(RenderListPoly &poly);

void draw_clipped_intensitiy_gourad_triangle(const RenderListPoly &poly);

void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color);

void scan_edges(IGouradEdge &long_edge, IGouradEdge &short_edge, bool handedness, RGBA color, const RenderListPoly &poly);
void scan_edges(CGouradEdge &left, CGouradEdge &right, bool handedness, RGBA color, const RenderListPoly &poly);

}

