#include "Rasterizer.h"
#include "../math/Core.h"

using Math::V2D;

#define SWAP(a, b, t) { t = a; a = b; b = t;}

namespace Graphics {

Pixel* p_frame_buffer;

A565Color **rgb_lookup;
int lookup_levels = 0;

int min_clip_y {0};
int min_clip_x {0};

int m_width {0};
int m_height {0};

float *inv_z_buffer = nullptr;

void init_rasterizer(int levels) {
    rgb_lookup = new A565Color*[levels];

    for (int i = 0; i < levels; i++)
        rgb_lookup[i] = new A565Color[65536];

    uint32_t r, g, b;
    float alhpa = 0;
    float delta_alpha = Math::EPSILON_E6 + 1 / ((float)(levels - 1));

    for (int level_index = 0; level_index < levels; level_index++) {
        for (uint32_t rgb_index = 0; rgb_index < 65536; rgb_index++) {
            ((A565Color) rgb_index).rgb565_from_16bit(r, g, b);

            r = (uint32_t) ((float) r * (float) alhpa);
            g = (uint32_t) ((float) g * (float) alhpa);
            b = (uint32_t) ((float) b * (float) alhpa);

            rgb_lookup[level_index][rgb_index] = rgba_bit(r << 3, g << 2, b << 3, 0xFF);
        }

        alhpa += delta_alpha;
    }

    lookup_levels = levels;
}

void cleanup_rasterizer() {
    for (int i = 0; i < lookup_levels; i++) {
        delete[] rgb_lookup[i];
    }

    delete[] rgb_lookup;
    delete[] inv_z_buffer;
}

void rast_set_frame_buffer(int width, int height, Pixel *frame_buffer) {
    if (m_width != width || height != m_height) {
        m_width = width;
        m_height = height;

        delete[] inv_z_buffer;
        inv_z_buffer = new float[m_width * m_height];
        p_frame_buffer = frame_buffer;
    }

    std::fill(inv_z_buffer, inv_z_buffer + m_width * m_height, 0);
}

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

void scan_edges(CGouradEdge &long_edge, CGouradEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t rp, gp, bp;

    if (y_start > m_height || y_end < 0)
        return;

    CGouradEdge &left = handedness ? short_edge : long_edge;
    CGouradEdge &right = handedness ? long_edge : short_edge;

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

        for(int x = x_start; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel(u * 16 -1 + 0.5f, v * 16 -1 + 0.5f);
            pixel.rgb565_from_16bit(rp, gp, bp);

            uint32_t r_col = (rp << 3) * (r / 255);
            uint32_t g_col = (gp << 2) * (g / 255);
            uint32_t b_col = (bp << 3) * (b / 255);

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

// Perfect perspective texture mapping
struct PTIINVZBEdge {
    float x;
    float dx_dy;
    int y_start;
    int y_end;

    // lighting
    float di_dy;
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PTIINVZBEdge() = default;

    PTIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        di_dy = (max_y_vert.i - min_y_vert.i) / y_dist;
        i = min_y_vert.i;

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
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

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

void scan_edges(PTIINVZBEdge &long_edge, PTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly) {
    float *iz_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    if (y_start > m_height || y_end < 0)
        return;

    PTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        float x_dist = right.x - left.x;
        float di_dx = (right.i - left.i) / x_dist;
        float i = left.i;

        float iz = left.iz;
        float iu = left.iu;
        float iv = left.iv;

        float x_start = left.x;
        float x_end = right.x;

        float diz_dx, diu_dx, div_dx;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = min_clip_x;
        }

        if (x_end > m_width)
            x_end = m_width;

        iz_ptr = inv_z_buffer + (y * m_width);

        for(int x = x_start; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * 16 -1 + 0.5f, (iv / iz) * 16 -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                p_frame_buffer[m_width * y + x].value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
                iz_ptr[x] = iz;
            }

            i += di_dx;

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.i += left.di_dy;
        right.i += right.di_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly) {
    if ((Math::f_cmp(poly.trans_verts[0].v.x, poly.trans_verts[1].v.x) && Math::f_cmp(poly.trans_verts[1].v.x, poly.trans_verts[2].v.x)) ||
        (Math::f_cmp(poly.trans_verts[0].v.y, poly.trans_verts[1].v.y) && Math::f_cmp(poly.trans_verts[1].v.y, poly.trans_verts[2].v.y)))
        return;

    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp = 0;

    if (poly.trans_verts[v1].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v1, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v2, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v1].v.y)
        SWAP(v1, v2, temp);

    float dx1 = poly.trans_verts[v2].v.x - poly.trans_verts[v0].v.x;
    float dy1 = poly.trans_verts[v2].v.y - poly.trans_verts[v0].v.y;

    float dx2 = poly.trans_verts[v1].v.x - poly.trans_verts[v0].v.x;
    float dy2 = poly.trans_verts[v1].v.y - poly.trans_verts[v0].v.y;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    PTIINVZBEdge bottom_to_top = PTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v2]);

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        PTIINVZBEdge bottom_to_middle = PTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        PTIINVZBEdge middle_to_top = PTIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    } else {
        PTIINVZBEdge bottom_to_middle = PTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);
        PTIINVZBEdge middle_to_top = PTIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    }
}



// Piecewise perspective texture mapping iinvzb
struct PPTIINVZBEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float di_dy;
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PPTIINVZBEdge() = default;

    PPTIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        di_dy = (max_y_vert.i - min_y_vert.i) / y_dist;
        i = min_y_vert.i;

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
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

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

void scan_edges(PPTIINVZBEdge &long_edge, PPTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly) {
    float *iz_ptr;
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    if (y_start > m_height || y_end < 0)
        return;

    PPTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PPTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        float x_dist = right.x - left.x;
        float di_dx = (right.i - left.i) / x_dist;
        float i = left.i;

        float ivl = left.iv / left.iz;
        float ivr = right.iv / right.iz;

        float iul = left.iu / left.iz;
        float iur = right.iu / right.iz;

        float iz = left.iz;
        float iu = iul;
        float iv = ivl;

        float diz_dx, diu_dx, div_dx;
        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (iur - iul) / x_dist;
            div_dx = (ivr - ivl) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (iur - iul);
            div_dx = (ivr - ivl);
        }


        float x_start = left.x;
        float x_end = right.x;

        if (x_start < min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = min_clip_x;
        }

        if (x_end > m_width)
            x_end = m_width;

        iz_ptr = inv_z_buffer + (y * m_width);

        for(int x = x_start; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                p_frame_buffer[m_width * y + x].value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
                iz_ptr[x] = iz;
            }

            i += di_dx;

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.i += left.di_dy;
        right.i += right.di_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly) {
    if ((Math::f_cmp(poly.trans_verts[0].v.x, poly.trans_verts[1].v.x) && Math::f_cmp(poly.trans_verts[1].v.x, poly.trans_verts[2].v.x)) ||
        (Math::f_cmp(poly.trans_verts[0].v.y, poly.trans_verts[1].v.y) && Math::f_cmp(poly.trans_verts[1].v.y, poly.trans_verts[2].v.y)))
        return;

    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp = 0;

    if (poly.trans_verts[v1].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v1, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v2, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v1].v.y)
        SWAP(v1, v2, temp);

    float dx1 = poly.trans_verts[v2].v.x - poly.trans_verts[v0].v.x;
    float dy1 = poly.trans_verts[v2].v.y - poly.trans_verts[v0].v.y;

    float dx2 = poly.trans_verts[v1].v.x - poly.trans_verts[v0].v.x;
    float dy2 = poly.trans_verts[v1].v.y - poly.trans_verts[v0].v.y;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    PPTIINVZBEdge bottom_to_top = PPTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v2]);

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        PPTIINVZBEdge bottom_to_middle = PPTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        PPTIINVZBEdge middle_to_top = PPTIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    } else {
        PPTIINVZBEdge bottom_to_middle = PPTIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);
        PPTIINVZBEdge middle_to_top = PPTIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    }
}


// Affine texture mapping
struct ATIINVZBEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // texturing
    float du_dy;
    float dv_dy;
    float u;
    float v;

    // lighting
    float di_dy;
    float i;

    // perspective z
    float iz;
    float diz_dy;

    ATIINVZBEdge() = default;

    ATIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
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

        if (y_start < min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            iz += diz_dy *-y_start;

            y_start = min_clip_y;
        }

        if (y_end > m_height) {
            y_end = m_height;
        }
    }
};

void scan_edges(ATIINVZBEdge &long_edge, ATIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly) {
    float *iz_ptr;
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    if (y_start > m_height || y_end < 0)
        return;

    ATIINVZBEdge &left = handedness ? short_edge : long_edge;
    ATIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        float x_dist = right.x - left.x;
        float di_dx = (right.i - left.i) / x_dist;
        float i = left.i;

        float du_dx = (right.u - left.u) / x_dist;
        float dv_dx = (right.v - left.v) / x_dist;

        float u = left.u;
        float v = left.v;

        float iz = left.iz;

        float diz_dx;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < min_clip_x) {
            i += di_dx * -x_start;

            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            iz += diz_dx * -x_start;

            x_start = min_clip_x;
        }

        if (x_end > m_width)
            x_end = m_width;

        iz_ptr = inv_z_buffer + (y * m_width);

        for(int x = x_start; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                p_frame_buffer[m_width * y + x].value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
                iz_ptr[x] = iz;
            }

            i += di_dx;

            iz += diz_dx;

            u += du_dx;
            v += dv_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.i += left.di_dy;
        right.i += right.di_dy;

        left.u += left.du_dy;
        left.v += left.dv_dy;

        right.u += right.du_dy;
        right.v += right.dv_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;
    }
}


void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly) {
    if ((Math::f_cmp(poly.trans_verts[0].v.x, poly.trans_verts[1].v.x) && Math::f_cmp(poly.trans_verts[1].v.x, poly.trans_verts[2].v.x)) ||
        (Math::f_cmp(poly.trans_verts[0].v.y, poly.trans_verts[1].v.y) && Math::f_cmp(poly.trans_verts[1].v.y, poly.trans_verts[2].v.y)))
        return;

    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp = 0;

    if (poly.trans_verts[v1].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v1, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v0].v.y)
        SWAP(v0, v2, temp);

    if (poly.trans_verts[v2].v.y < poly.trans_verts[v1].v.y)
        SWAP(v1, v2, temp);

    float dx1 = poly.trans_verts[v2].v.x - poly.trans_verts[v0].v.x;
    float dy1 = poly.trans_verts[v2].v.y - poly.trans_verts[v0].v.y;

    float dx2 = poly.trans_verts[v1].v.x - poly.trans_verts[v0].v.x;
    float dy2 = poly.trans_verts[v1].v.y - poly.trans_verts[v0].v.y;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    ATIINVZBEdge bottom_to_top = ATIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v2]);

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        ATIINVZBEdge bottom_to_middle = ATIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        ATIINVZBEdge middle_to_top = ATIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    } else {
        ATIINVZBEdge bottom_to_middle = ATIINVZBEdge(poly.trans_verts[v0], poly.trans_verts[v1]);
        ATIINVZBEdge middle_to_top = ATIINVZBEdge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly);
    }
}


// Non-textured rastization methods
}

