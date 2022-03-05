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


void init_rasterizer(int levels);
void cleanup_rasterizer();
void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

// TODO Old legecy functions that that will be replaced once all newer ones are implemented
void draw_colored_gouraud_triangle(RenderListPoly &poly);

////////// Perfect perspective texture mapping //////////

void draw_perspective_textured_triangle_fsinvzb(RenderListPoly &poly);
void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly);

void draw_perspective_textured_triangle_fs(RenderListPoly &poly);
void draw_perspective_textured_triangle_i(RenderListPoly &poly);

////////// Piecewise perspective texture mapping //////////

// Draws triangle with piecewise perspective texture mapping with flat shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_fsinvzb(RenderListPoly &poly);

// Draws triangle with piecewise perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly);

// Draws triangle with piecewise perspective texture mapping with flat shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_fs(RenderListPoly &poly);
//
// Draws triangle with piecewise perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_i(RenderListPoly &poly);

////////// Affine texture mapping //////////

void draw_affine_textured_triangle_fsinvzb(RenderListPoly &poly);
void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly);

void draw_affine_textured_triangle_fs(RenderListPoly &poly);
void draw_affine_textured_triangle_i(RenderListPoly &poly);

// Non-textured rastization methods
void draw_triangle_s(RenderListPoly &poly);
void draw_triangle_fs(RenderListPoly &poly);
void draw_triangle_i(RenderListPoly &poly);

void draw_triangle_sinvzb(RenderListPoly &poly);
void draw_triangle_fsinvzb(RenderListPoly &poly);
void draw_triangle_iinvzb(RenderListPoly &poly);

void scan_edges(CGouradEdge &left, CGouradEdge &right, bool handedness, A565Color color, const RenderListPoly &poly);

}

