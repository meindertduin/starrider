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

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2]);
        CGouradEdge bottom_to_middle = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v1],
                poly.lit_color[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2]);
        CGouradEdge middle_to_top = CGouradEdge(poly.trans_verts[v1], poly.lit_color[v1], poly.trans_verts[v2],
                poly.lit_color[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    } else {
        CGouradEdge bottom_to_top = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v2],
                poly.lit_color[v2]);
        CGouradEdge bottom_to_middle = CGouradEdge(poly.trans_verts[v0], poly.lit_color[v0], poly.trans_verts[v1],
                poly.lit_color[v1]);
        CGouradEdge middle_to_top = CGouradEdge(poly.trans_verts[v1], poly.lit_color[v1], poly.trans_verts[v2],
                poly.lit_color[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
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

    IGouradEdge bottom_to_top = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v2]);

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        IGouradEdge bottom_to_middle = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        IGouradEdge middle_to_top = IGouradEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    } else {
        IGouradEdge bottom_to_middle = IGouradEdge(poly.trans_verts[v0], poly.trans_verts[v1]);
        IGouradEdge middle_to_top = IGouradEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    }
}

void scan_edges(IGouradEdge &long_edge, IGouradEdge &short_edge, bool handedness, RGBA color, const RenderListPoly &poly) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    if (y_start > m_height || y_end < 0)
        return;

    IGouradEdge &left = handedness ? short_edge : long_edge;
    IGouradEdge &right = handedness ? long_edge : short_edge;

    if (y_start < min_clip_y) {
        left.x += left.x_step * -y_start;
        right.x += right.x_step * -y_start;

        left.i += left.di_dy * -y_start;
        right.i += right.di_dy * -y_start;

        left.u += left.du_dy * -y_start;
        left.v += left.dv_dy * -y_start;

        right.u += right.du_dy * -y_start;
        right.v += right.dv_dy * -y_start;

        y_start = min_clip_y;
    }

    if (y_end > m_height) {
        y_end = m_height;
    }


    for(int y = y_start; y < y_end; y++) {
        float x_dist = right.x - left.x;
        float dix = (right.i - left.i) / x_dist;
        float i = left.i;

        float du = (right.u - left.u) / x_dist;
        float dv = (right.v - left.v) / x_dist;

        float u = left.u;
        float v = left.v;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < min_clip_x) {
            i += dix * -x_start;

            u += du * -x_start;
            v += dv * -x_start;

            x_start = min_clip_x;
        }

        if (x_end > m_width)
            x_end = m_width;

        for(int x = x_start; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel(u * 16 -1 + 0.5f, v * 16 -1 + 0.5f);
            p_frame_buffer[m_width * y + x].value = rgba_bit(pixel.rgba.red * i, pixel.rgba.green * i, pixel.rgba.blue * i, 0xFF);;

            i += dix;

            u += du;
            v += dv;
        }

        left.x += left.x_step;
        right.x += right.x_step;

        left.i += left.di_dy;
        right.i += right.di_dy;

        left.u += left.du_dy;
        left.v += left.dv_dy;

        right.u += right.du_dy;
        right.v += right.dv_dy;
    }
}

void scan_edges(CGouradEdge &long_edge, CGouradEdge &short_edge, bool handedness, RGBA color, const RenderListPoly &poly) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    if (y_start > m_height || y_end < 0)
        return;

    CGouradEdge &left = handedness ? short_edge : long_edge;
    CGouradEdge &right = handedness ? long_edge : short_edge;

    if (y_start < min_clip_y) {
        left.x += left.x_step * -y_start;
        right.x += right.x_step * -y_start;

        left.r += left.dr_dy * -y_start;
        left.g += left.dg_dy * -y_start;
        left.b += left.db_dy * -y_start;

        right.r += right.dr_dy * -y_start;
        right.g += right.dg_dy * -y_start;
        right.b += right.db_dy * -y_start;

        left.u += left.du_dy * -y_start;
        left.v += left.dv_dy * -y_start;

        right.u += right.du_dy * -y_start;
        right.v += right.dv_dy * -y_start;

        y_start = min_clip_y;
    }

    if (y_end > m_height) {
        y_end = m_height;
    }

    for(int y = y_start; y < y_end; y++) {
        float x_dist = right.x - left.x;
        float drx = (right.r - left.r) / x_dist;
        float r = left.r;

        float dgx = (right.g - left.g) / x_dist;
        float g = left.g;

        float dbx = (right.b - left.b) / x_dist;
        float b = left.b;

        float du = (right.u - left.u) / x_dist;
        float dv = (right.v - left.v) / x_dist;

        float u = left.u;
        float v = left.v;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < min_clip_x) {
            r += drx * -x_start;
            g += dgx * -x_start;
            b += dbx * -x_start;

            u += du * -x_start;
            v += dv * -x_start;

            x_start = min_clip_x;
        }

        if (x_end > m_width)
            x_end = m_width;

        for(int x = left.x; x < right.x; x++) {
            auto pixel = poly.texture->get_pixel(u * 16 -1 + 0.5f, v * 16 -1 + 0.5f);
            uint32_t r_col = pixel.rgba.red * (r / 255);
            uint32_t g_col = pixel.rgba.green * (g / 255);
            uint32_t b_col = pixel.rgba.blue * (b / 255);

            p_frame_buffer[m_width * y + x].value = rgba_bit(r_col, g_col, b_col, 0xFF);

            r += drx;
            g += dgx;
            b += dbx;

            u += du;
            v += dv;
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

        right.u += right.du_dy;
        right.v += right.dv_dy;
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

