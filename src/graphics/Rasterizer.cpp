#include "Rasterizer.h"
#include "../math/Core.h"

using Math::V2D;

namespace Graphics {

Pixel* p_frame_buffer;

int min_clip_y {0};
int min_clip_x {0};

int m_width {0};
int m_height {0};

void draw_colored_gouraud_triangle(RenderListPoly &poly) {
    if ((Math::f_cmp(poly.trans_verts[0].v.x, poly.trans_verts[1].v.x) && Math::f_cmp(poly.trans_verts[1].v.x, poly.trans_verts[2].v.x)) ||
        (Math::f_cmp(poly.trans_verts[0].v.y, poly.trans_verts[1].v.y) && Math::f_cmp(poly.trans_verts[1].v.y, poly.trans_verts[2].v.y)))
        return;

    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp = 0;

    if (poly.trans_verts[v1].v.y < poly.trans_verts[v0].v.y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v0].v.y) {
        temp = v0;
        v0 = v2;
        v2 = temp;
    }


    if (poly.trans_verts[v2].v.y < poly.trans_verts[v1].v.y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }

    float dx1 = poly.trans_verts[v2].v.x - poly.trans_verts[v0].v.x;
    float dy1 = poly.trans_verts[v2].v.y - poly.trans_verts[v0].v.y;

    float dx2 = poly.trans_verts[v1].v.x - poly.trans_verts[v0].v.x;
    float dy2 = poly.trans_verts[v1].v.y - poly.trans_verts[v0].v.y;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    auto gradients = Gradients(poly.trans_verts[v0], poly.trans_verts[v1], poly.trans_verts[v2]);

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2], gradients, v0);
        CGouradEdge bottom_to_middle = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v1],
                poly.lit_color[v1], gradients, v0);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, gradients);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2], gradients, v0);
        CGouradEdge middle_to_top = CGouradEdge(poly.trans_verts[v1], poly.lit_color[v1], poly.trans_verts[v2],
                poly.lit_color[v2], gradients, v1);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, gradients);
    } else {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2], gradients, v0);
        CGouradEdge bottom_to_middle = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v1],
                poly.lit_color[v1], gradients, v0);
        CGouradEdge middle_to_top = CGouradEdge(poly.trans_verts[v1], poly.lit_color[v1], poly.trans_verts[v2],
                poly.lit_color[v2], gradients, v1);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, gradients);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, gradients);
    }
}

void draw_intensity_gouraud_triangle(RenderListPoly &poly) {
    if ((Math::f_cmp(poly.trans_verts[0].v.x, poly.trans_verts[1].v.x) && Math::f_cmp(poly.trans_verts[1].v.x, poly.trans_verts[2].v.x)) ||
        (Math::f_cmp(poly.trans_verts[0].v.y, poly.trans_verts[1].v.y) && Math::f_cmp(poly.trans_verts[1].v.y, poly.trans_verts[2].v.y)))
        return;

    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp = 0;

    if (poly.trans_verts[v1].v.y < poly.trans_verts[v0].v.y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v0].v.y) {
        temp = v0;
        v0 = v2;
        v2 = temp;
    }


    if (poly.trans_verts[v2].v.y < poly.trans_verts[v1].v.y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }

    float dx1 = poly.trans_verts[v2].v.x - poly.trans_verts[v0].v.x;
    float dy1 = poly.trans_verts[v2].v.y - poly.trans_verts[v0].v.y;

    float dx2 = poly.trans_verts[v1].v.x - poly.trans_verts[v0].v.x;
    float dy2 = poly.trans_verts[v1].v.y - poly.trans_verts[v0].v.y;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        IGouradEdge bottom_to_top = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v2]);
        IGouradEdge bottom_to_middle = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        IGouradEdge bottom_to_top = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v2]);
        IGouradEdge middle_to_top = IGouradEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color);
    } else {
        IGouradEdge bottom_to_top = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v2]);
        IGouradEdge bottom_to_middle = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v1]);
        IGouradEdge middle_to_top = IGouradEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color);
    }
}

void scan_edges(IGouradEdge &long_edge, IGouradEdge &short_edge, bool handedness, RGBA color) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    IGouradEdge &left = handedness ? short_edge : long_edge;
    IGouradEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        float dix = (right.i - left.i) / (right.x - left.x);
        float i = left.i;
        for(int x = left.x; x < right.x; x++) {
            if (x > 0 && y > 0 && x < m_width && y < m_height)
                p_frame_buffer[m_width * y + x].value = color.to_argb_bit(i);
            i += dix;
        }

        left.x += left.x_step;
        right.x += right.x_step;

        left.i += left.di_dy;
        right.i += right.di_dy;
    }
}

void scan_edges(CGouradEdge &long_edge, CGouradEdge &short_edge, bool handedness, RGBA color, const RenderListPoly &poly, Gradients &gradients) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    CGouradEdge &left = handedness ? short_edge : long_edge;
    CGouradEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        float drx = (right.r - left.r) / (right.x - left.x);
        float r = left.r;

        float dgx = (right.g - left.g) / (right.x - left.x);
        float g = left.g;

        float dbx = (right.b - left.b) / (right.x - left.x);
        float b = left.b;

        float du = (right.u - left.u) / (right.x - left.x);
        float dv = (right.v - left.v) / (right.x - left.x);

        float u = left.u;
        float v = left.v;

        float text_coord_x_xstep = gradients.text_coord_x_xstep;
        float text_coord_y_xstep = gradients.text_coord_y_xstep;

        for(int x = left.x; x < right.x; x++) {
            if (x > 0 && y > 0 && x < m_width && y < m_height) {
                auto pixel = poly.texture->get_pixel(u * 16 -1, v * 16 -1);
                p_frame_buffer[m_width * y + x].value = rgba_bit(pixel.rgba.red, pixel.rgba.green, pixel.rgba.green, 0xFF);
            }
            r += drx;
            g += dgx;
            b += dbx;

            u += text_coord_x_xstep;
            v += text_coord_y_xstep;
        }

        left.x += left.x_step;
        right.x += right.x_step;

        left.r += left.dr_dy;
        left.g += left.dg_dy;
        left.b += left.db_dy;

        right.r += right.dr_dy;
        right.g += right.dg_dy;
        right.b += right.db_dy;

        left.u += left.du_dy;
        left.v += left.dv_dy;

        right.u += left.du_dy;
        right.v += left.dv_dy;
    }
}

void rast_set_frame_buffer(int width, int height, Pixel *frame_buffer) {
    if (m_width != width || height != m_height) {
        m_width = width;
        m_height = height;

        p_frame_buffer = frame_buffer;
    }
}

}

