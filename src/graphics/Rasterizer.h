#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

namespace Graphics {

extern int min_clip_y;
extern int min_clip_x;

extern int m_width;
extern int m_height;

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

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

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

        // float 1/z perspective
        float tz_max = 1.0f / (max_y_vert.v.z);
        float tz_min = 1.0f / (min_y_vert.v.z);
        diz_dy = (tz_max - tz_min) / y_dist;
        iz = tz_min;

        float iu_max = (max_y_vert.t.x) / (max_y_vert.v.z);
        float iu_min = (min_y_vert.t.x) / (min_y_vert.v.z);

        float iv_max = (max_y_vert.t.y) / (max_y_vert.v.z);
        float iv_min = (min_y_vert.t.y) / (min_y_vert.v.z);

        iu = iu_min;
        iv = iv_min;

        diu_dy = (iu_max - iu_min) / y_dist;
        div_dy = (iv_max - iv_min) / y_dist;

        if (y_start < min_clip_y) {
            x += x_step * -y_start;
            i += di_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = min_clip_y;
        }

        if (y_end > m_height) {
            y_end = m_height;
        }
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

    CGouradEdge(const Vertex4D &min_y_vert, A565Color min_y_vert_col, const Vertex4D &max_y_vert, A565Color max_y_vert_col) {
        y_start = min_y_vert.v.y;
        y_end = max_y_vert.v.y;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x- min_y_vert.v.x;

        x_step = x_dist / y_dist;
        x = min_y_vert.v.x;

        uint32_t max_y_r, max_y_g, max_y_b, min_y_r, min_y_g, min_y_b;

        max_y_vert_col.rgb888_from_16bit(max_y_r, max_y_g, max_y_b);
        min_y_vert_col.rgb888_from_16bit(min_y_r, min_y_g, min_y_b);

        dr_dy = (max_y_r - min_y_r) / y_dist;
        dg_dy = (max_y_g - min_y_g) / y_dist;
        db_dy = (max_y_b - min_y_b) / y_dist;

        r = min_y_r;
        g = min_y_g;
        b = min_y_b;

        u = min_y_vert.t.x;
        v = min_y_vert.t.y;

        du_dy = ((max_y_vert.t.x - min_y_vert.t.x) / y_dist);
        dv_dy = ((max_y_vert.t.y - min_y_vert.t.y) / y_dist);

        if (y_start < min_clip_y) {
            x += x_step * -y_start;

            r += dr_dy * -y_start;
            g += dg_dy * -y_start;
            b += db_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            y_start = min_clip_y;
        }

        if (y_end > m_height) {
            y_end = m_height;
        }
    }
};

void draw_colored_gouraud_triangle(RenderListPoly &poly);

void draw_intensity_gouraud_triangle(RenderListPoly &poly);

void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

void scan_edges(IGouradEdge &long_edge, IGouradEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly);
void scan_edges(CGouradEdge &left, CGouradEdge &right, bool handedness, A565Color color, const RenderListPoly &poly);

void init_rasterizer(int levels);
void cleanup_rasterizer();

}

