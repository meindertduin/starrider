#include "Rasterizer.h"
#include "../math/Core.h"

using Math::V2D;

#define SWAP(a, b, t) { t = a; a = b; b = t; }

namespace Graphics {

A565Color **rgb_lookup;
int lookup_levels = 0;

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

            rgb_lookup[level_index][rgb_index] = A565Color(r, g, b);
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
}

////////// Perfect perspective texture mapping //////////

struct PTFSINVZBEdge {
    float x;
    float dx_dy;
    int y_start;
    int y_end;

    // lighting
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PTFSINVZBEdge() = default;

    PTFSINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PTFSINVZBEdge &long_edge, PTFSINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, iz, iu, iv, x_start, x_end, diz_dx, diu_dx, div_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PTFSINVZBEdge &left = handedness ? short_edge : long_edge;
    PTFSINVZBEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        iz = left.iz;
        iu = left.iu;
        iv = left.iv;

        x_start = left.x;
        x_end = right.x;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * poly.texture->width -1 + 0.5f, (iv / iz) * poly.texture->width -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);
                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

                iz_ptr[x] = iz;
            }

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_perspective_textured_triangle_fsinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PTFSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PTFSINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PTFSINVZBEdge{ poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PTFSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PTFSINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

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

    PTIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PTIINVZBEdge &long_edge, PTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, iz, iu, iv, x_start, x_end, diz_dx, diu_dx, div_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        iz = left.iz;
        iu = left.iu;
        iv = left.iv;

        x_start = left.x;
        x_end = right.x;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (y * rc.max_clip_x);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
           if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * poly.texture->width -1 + 0.5f, (iv / iz) * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
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


void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    PTIINVZBEdge bottom_to_top = PTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        PTIINVZBEdge bottom_to_middle = PTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        PTIINVZBEdge middle_to_top = PTIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        PTIINVZBEdge bottom_to_middle = PTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        PTIINVZBEdge middle_to_top = PTIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

void scan_edges(PTIINVZBEdge &long_edge, PTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, float alpha, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, iz, iu, iv, x_start, x_end, diz_dx, diu_dx, div_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        iz = left.iz;
        iu = left.iu;
        iv = left.iv;

        x_start = left.x;
        x_end = right.x;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (y * rc.max_clip_x);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * poly.texture->width -1 + 0.5f, (iv / iz) * poly.texture->width -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                auto current_pixel = (screen_buffer_ptr + x);

                auto red = (uint32_t)((alpha * (r << 3) * i) + ((1 - alpha) * (current_pixel->red))) & 0x000000FF;
                auto green = (uint32_t)((alpha * (g << 2) * i) + ((1 - alpha) * (current_pixel->green))) & 0x000000FF;
                auto blue = (uint32_t)((alpha * (b << 3) * i) + ((1 - alpha) * (current_pixel->blue))) & 0x000000FF;

                (screen_buffer_ptr + x)->value = rgba_bit(red, green, blue, 0xFF);
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

void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly, float alpha, RenderContext &rc) {
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

    auto bottom_to_top = PTIINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PTIINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PTIINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    } else {
        auto bottom_to_middle = PTIINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PTIINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    }
}

struct PTFSEdge {
    float x;
    float dx_dy;
    int y_start;
    int y_end;

    // lighting
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PTFSEdge() = default;

    PTFSEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PTFSEdge &long_edge, PTFSEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, iz, iu, iv, x_start, x_end, diz_dx, diu_dx, div_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PTFSEdge &left = handedness ? short_edge : long_edge;
    PTFSEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        iz = left.iz;
        iu = left.iu;
        iv = left.iv;

        x_start = left.x;
        x_end = right.x;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * poly.texture->width -1 + 0.5f, (iv / iz) * poly.texture->width -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            rc.frame_buffer[rc.max_clip_x * y + x].value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_perspective_textured_triangle_fs(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PTFSEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PTFSEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PTFSEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PTFSEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PTFSEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct PTIEdge {
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

    PTIEdge() = default;

    PTIEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PTIEdge &long_edge, PTIEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    Pixel *screen_buffer_ptr;

    float x_dist, di_dx, i, iz, iu, iv, x_start, x_end, diz_dx, diu_dx, div_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PTIEdge &left = handedness ? short_edge : long_edge;
    PTIEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        iz = left.iz;
        iu = left.iu;
        iv = left.iv;

        x_start = left.x;
        x_end = right.x;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;

            diu_dx = (right.iu - left.iu) / x_dist;
            div_dx = (right.iv - left.iv) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);

            diu_dx = (right.iu - left.iu);
            div_dx = (right.iv - left.iv);
        }

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        screen_buffer_ptr = rc.frame_buffer + (y * rc.max_clip_x);

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift((iu / iz) * poly.texture->width -1 + 0.5f, (iv / iz) * poly.texture->width -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

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


void draw_perspective_textured_triangle_i(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PTIEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PTIEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PTIEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PTIEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PTIEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}



////////// Piecewise perspective texture mapping //////////

struct PPTFSINVZBEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PPTFSINVZBEdge() = default;

    PPTFSINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PPTFSINVZBEdge &long_edge, PPTFSINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    float x_dist, di_dx, ivl, ivr, iul, iur, iz, iu, iv, diz_dx, diu_dx, div_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PPTFSINVZBEdge &left = handedness ? short_edge : long_edge;
    PPTFSINVZBEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;

        ivl = left.iv / left.iz;
        ivr = right.iv / right.iz;

        iul = left.iu / left.iz;
        iur = right.iu / right.iz;

        iz = left.iz;
        iu = iul;
        iv = ivl;

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

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
                iz_ptr[x] = iz;
            }

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_piecewise_textured_triangle_fsinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PPTFSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PPTFSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PPTFSINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PPTFSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PPTFSINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

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

    PPTIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PPTIINVZBEdge &long_edge, PPTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    float x_dist, di_dx, i, ivl, ivr, iul, iur, iz, iu, iv, diz_dx, diu_dx, div_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PPTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PPTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        ivl = left.iv / left.iz;
        ivr = right.iv / right.iz;

        iul = left.iu / left.iz;
        iur = right.iu / right.iz;

        iz = left.iz;
        iu = iul;
        iv = ivl;

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

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
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

void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PPTIINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PPTIINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PPTIINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PPTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PPTIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

void scan_edges(PPTIINVZBEdge &long_edge, PPTIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, float alpha, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    float x_dist, di_dx, i, ivl, ivr, iul, iur, iz, iu, iv, diz_dx, diu_dx, div_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PPTIINVZBEdge &left = handedness ? short_edge : long_edge;
    PPTIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        ivl = left.iv / left.iz;
        ivr = right.iv / right.iz;

        iul = left.iu / left.iz;
        iur = right.iu / right.iz;

        iz = left.iz;
        iu = iul;
        iv = ivl;

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

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                auto current_pixel = (screen_buffer_ptr + x);

                auto red = (uint32_t)((alpha * (r << 3) * i) + ((1 - alpha) * (current_pixel->red))) & 0x000000FF;
                auto green = (uint32_t)((alpha * (g << 2) * i) + ((1 - alpha) * (current_pixel->green))) & 0x000000FF;
                auto blue = (uint32_t)((alpha * (b << 3) * i) + ((1 - alpha) * (current_pixel->blue))) & 0x000000FF;

                (screen_buffer_ptr + x)->value = rgba_bit(red, green, blue, 0xFF);

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



void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly, float alpha, RenderContext &rc) {
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

    auto bottom_to_top = PPTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PPTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PPTIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    } else {
        auto bottom_to_middle = PPTIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PPTIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    }
}

struct PPTFSEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float i;

    // perspective z
    float iz;
    float diz_dy;

    // perspective texturing
    float iu;
    float iv;

    float diu_dy;
    float div_dy;

    PPTFSEdge() = default;

    PPTFSEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PPTFSEdge &long_edge, PPTFSEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    float x_dist, ivl, ivr, iul, iur, iz, iu, iv, diz_dx, diu_dx, div_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PPTFSEdge &left = handedness ? short_edge : long_edge;
    PPTFSEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        ivl = left.iv / left.iz;
        ivr = right.iv / right.iz;

        iul = left.iu / left.iz;
        iur = right.iu / right.iz;

        iz = left.iz;
        iu = iul;
        iv = ivl;

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

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

            iz += diz_dx;

            iu += diu_dx;
            iv += div_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;

        left.iu += left.diu_dy;
        right.iu += right.diu_dy;

        left.iv += left.div_dy;
        right.iv += right.div_dy;
    }
}


void draw_piecewise_textured_triangle_fs(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PPTFSEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PPTFSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PPTFSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PPTFSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PPTFSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct PPTIEdge {
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

    PPTIEdge() = default;

    PPTIEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            iz += diz_dy *-y_start;

            iu += diu_dy *-y_start;
            iv += div_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(PPTIEdge &long_edge, PPTIEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    uint32_t r, g, b;

    float x_dist, di_dx, i, ivl, ivr, iul, iur, iz, iu, iv, diz_dx, diu_dx, div_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    PPTIEdge &left = handedness ? short_edge : long_edge;
    PPTIEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        ivl = left.iv / left.iz;
        ivr = right.iv / right.iz;

        iul = left.iu / left.iz;
        iur = right.iu / right.iz;

        iz = left.iz;
        iu = iul;
        iv = ivl;

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

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            iz += diz_dx * -x_start;

            iu += diu_dx * -x_start;
            iv += div_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        screen_buffer_ptr = rc.frame_buffer + (rc.max_clip_x * y);

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift(iu * poly.texture->width -1 + 0.5f, iv * poly.texture->height -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

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


void draw_piecewise_textured_triangle_i(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = PPTIEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = PPTIEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = PPTIEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = PPTIEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = PPTIEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}





////////// Affine texture mapping //////////

struct ATFSINVZBEdge {
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
    float i;

    // perspective z
    float iz;
    float diz_dy;

    ATFSINVZBEdge() = default;

    ATFSINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            iz += diz_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(ATFSINVZBEdge &long_edge, ATFSINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, du_dx, dv_dx, u, v, iz, diz_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    ATFSINVZBEdge &left = handedness ? short_edge : long_edge;
    ATFSINVZBEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        du_dx = (right.u - left.u) / x_dist;
        dv_dx = (right.v - left.v) / x_dist;

        u = left.u;
        v = left.v;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
                iz_ptr[x] = iz;
            }

            iz += diz_dx;

            u += du_dx;
            v += dv_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.u += left.du_dy;
        left.v += left.dv_dy;

        right.u += right.du_dy;
        right.v += right.dv_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;
    }
}


void draw_affine_textured_triangle_fsinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = ATFSINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = ATFSINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = ATFSINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = ATFSINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = ATFSINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

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

    ATIINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            iz += diz_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(ATIINVZBEdge &long_edge, ATIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, du_dx, dv_dx, u, v, iz, diz_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    ATIINVZBEdge &left = handedness ? short_edge : long_edge;
    ATIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        du_dx = (right.u - left.u) / x_dist;
        dv_dx = (right.v - left.v) / x_dist;

        u = left.u;
        v = left.v;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);
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


void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = ATIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = ATIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

void scan_edges(ATIINVZBEdge &long_edge, ATIINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, float alpha, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, du_dx, dv_dx, u, v, iz, diz_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    ATIINVZBEdge &left = handedness ? short_edge : long_edge;
    ATIINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        du_dx = (right.u - left.u) / x_dist;
        dv_dx = (right.v - left.v) / x_dist;

        u = left.u;
        v = left.v;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
                pixel.rgb565_from_16bit(r, g, b);

                auto current_pixel = (screen_buffer_ptr + x);

                auto red = (uint32_t)((alpha * (r << 3) * i) + ((1 - alpha) * (current_pixel->red))) & 0x000000FF;
                auto green = (uint32_t)((alpha * (g << 2) * i) + ((1 - alpha) * (current_pixel->green))) & 0x000000FF;
                auto blue = (uint32_t)((alpha * (b << 3) * i) + ((1 - alpha) * (current_pixel->blue))) & 0x000000FF;

                (screen_buffer_ptr + x)->value = rgba_bit(red, green, blue, 0xFF);
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

void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly, float alpha, RenderContext &rc) {
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

    auto bottom_to_top = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = ATIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    } else {
        auto bottom_to_middle = ATIINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = ATIINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, alpha, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, alpha, rc);
    }

}

struct ATFSEdge {
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
    float i;

    ATFSEdge() = default;

    ATFSEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        i = min_y_vert.i;

        u = min_y_vert.t.x;
        v = min_y_vert.t.y;

        du_dy = ((max_y_vert.t.x - min_y_vert.t.x) / y_dist);
        dv_dy = ((max_y_vert.t.y - min_y_vert.t.y) / y_dist);

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

             y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(ATFSEdge &long_edge, ATFSEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, du_dx, dv_dx, u, v;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    ATFSEdge &left = handedness ? short_edge : long_edge;
    ATFSEdge &right = handedness ? long_edge : short_edge;

    float i = left.i;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        du_dx = (right.u - left.u) / x_dist;
        dv_dx = (right.v - left.v) / x_dist;

        u = left.u;
        v = left.v;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

            u += du_dx;
            v += dv_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.u += left.du_dy;
        left.v += left.dv_dy;

        right.u += right.du_dy;
        right.v += right.dv_dy;
    }
}


void draw_affine_textured_triangle_fs(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = ATFSEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = ATFSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = ATFSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = ATFSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = ATFSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct ATIEdge {
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

    ATIEdge() = default;

    ATIEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            u += du_dy * -y_start;
            v += dv_dy * -y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(ATIEdge &long_edge, ATIEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, du_dx, dv_dx, u, v;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    ATIEdge &left = handedness ? short_edge : long_edge;
    ATIEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        du_dx = (right.u - left.u) / x_dist;
        dv_dx = (right.v - left.v) / x_dist;

        u = left.u;
        v = left.v;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            u += du_dx * -x_start;
            v += dv_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            auto pixel = poly.texture->get_pixel_by_shift(u * poly.texture->width -1 + 0.5f, v * poly.texture->height -1 + 0.5f);
            pixel.rgb565_from_16bit(r, g, b);

            (screen_buffer_ptr + x)->value = rgba_bit((r << 3) * i, (g << 2) * i, (b << 3) * i, 0xFF);

            i += di_dx;

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
    }
}


void draw_affine_textured_triangle_i(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = ATIEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = ATIEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = ATIEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = ATIEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = ATIEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}


////////// Non-textured rastization methods //////////

struct SEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    SEdge() = default;

    SEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;


        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(SEdge &long_edge, SEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    SEdge &left = handedness ? short_edge : long_edge;
    SEdge &right = handedness ? long_edge : short_edge;

    auto pixel_argb = poly.color.rgba_bit();

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            (screen_buffer_ptr + x)->value = pixel_argb;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;
    }
}


void draw_triangle_s(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = SEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = SEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = SEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = SEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = SEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct FSEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float i;

    FSEdge() = default;

    FSEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        i = min_y_vert.i;

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(FSEdge &long_edge, FSEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    FSEdge &left = handedness ? short_edge : long_edge;
    FSEdge &right = handedness ? long_edge : short_edge;

    auto pixel_argb = poly.color.rgba_bit(left.i);

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            (screen_buffer_ptr + x)->value = pixel_argb;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;
    }
}


void draw_triangle_fs(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = FSEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = FSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = FSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = FSEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = FSEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct IEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float di_dy;
    float i;

    IEdge() = default;

    IEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        di_dy = (max_y_vert.i - min_y_vert.i) / y_dist;
        i = min_y_vert.i;

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;
            i += di_dy * -y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(IEdge &long_edge, IEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    IEdge &left = handedness ? short_edge : long_edge;
    IEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {

            (screen_buffer_ptr + x)->value = poly.color.rgba_bit(i);

            i += di_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.i += left.di_dy;
        right.i += right.di_dy;
    }
}


void draw_triangle_i(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = IEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = IEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = IEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = IEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = IEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct SINVZBEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // perspective z
    float iz;
    float diz_dy;

    SINVZBEdge() = default;

    SINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        // float 1/z perspective
        float tz_max = 1.0f / (max_y_vert.v.z);
        float tz_min = 1.0f / (min_y_vert.v.z);
        diz_dy = (tz_max - tz_min) / y_dist;
        iz = tz_min;

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            iz += diz_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(SINVZBEdge &long_edge, SINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, iz, diz_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    SINVZBEdge &left = handedness ? short_edge : long_edge;
    SINVZBEdge &right = handedness ? long_edge : short_edge;

    auto pixel_argb = poly.color.rgba_bit();

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        float x_start = left.x;
        float x_end = right.x;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;
        iz_ptr = rc.inv_z_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                (screen_buffer_ptr + x)->value = pixel_argb;
                iz_ptr[x] = iz;
            }

            iz += diz_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;
    }
}


void draw_triangle_sinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = SINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = SINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = SINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = SINVZBEdge {poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = SINVZBEdge {poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct FSINVZBEdge {
    float x;
    float dx_dy;

    int y_start;
    int y_end;

    // lighting
    float i;

    // perspective z
    float iz;
    float diz_dy;

    FSINVZBEdge() = default;

    FSINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
        y_start = min_y_vert.v.y + 0.5f;
        y_end = max_y_vert.v.y + 0.5f;

        float y_dist = max_y_vert.v.y - min_y_vert.v.y;
        float x_dist = max_y_vert.v.x - min_y_vert.v.x;

        dx_dy = x_dist / y_dist;
        x = min_y_vert.v.x;

        i = min_y_vert.i;

        // float 1/z perspective
        float tz_max = 1.0f / (max_y_vert.v.z);
        float tz_min = 1.0f / (min_y_vert.v.z);
        diz_dy = (tz_max - tz_min) / y_dist;
        iz = tz_min;

        if (y_start < rc.min_clip_y) {
            x += dx_dy * -y_start;

            iz += diz_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(FSINVZBEdge &long_edge, FSINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, iz, diz_dx;

    uint32_t r, g, b;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    FSINVZBEdge &left = handedness ? short_edge : long_edge;
    FSINVZBEdge &right = handedness ? long_edge : short_edge;

    auto pixel_argb = poly.color.rgba_bit(left.i);

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                (screen_buffer_ptr + x)->value = pixel_argb;
                iz_ptr[x] = iz;
            }

            iz += diz_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;
    }
}


void draw_triangle_fsinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = FSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = FSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = FSINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = FSINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = FSINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

struct IINVZBEdge {
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

    IINVZBEdge() = default;

    IINVZBEdge(const Vertex4D &min_y_vert, const Vertex4D &max_y_vert, RenderContext &rc) {
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

        if (y_start < rc.min_clip_y) {
            iz += diz_dy *-y_start;

            y_start = rc.min_clip_y;
        }

        if (y_end > rc.max_clip_y) {
            y_end = rc.max_clip_y;
        }
    }
};

void scan_edges(IINVZBEdge &long_edge, IINVZBEdge &short_edge, bool handedness, A565Color color, const RenderListPoly &poly, RenderContext &rc) {
    float *iz_ptr;
    Pixel *screen_buffer_ptr;

    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    float x_dist, di_dx, i, iz, diz_dx;

    if (y_start > rc.max_clip_y || y_end < 0)
        return;

    IINVZBEdge &left = handedness ? short_edge : long_edge;
    IINVZBEdge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        x_dist = right.x - left.x;
        di_dx = (right.i - left.i) / x_dist;
        i = left.i;

        iz = left.iz;

        if (x_dist > 0) {
            diz_dx = (right.iz - left.iz) / x_dist;
        } else {
            diz_dx = (right.iz - left.iz);
        }

        float x_start = left.x;
        float x_end = right.x;

        if (x_start < rc.min_clip_x) {
            i += di_dx * -x_start;
            iz += diz_dx * -x_start;

            x_start = rc.min_clip_x;
        }

        if (x_end > rc.max_clip_x)
            x_end = rc.max_clip_x;

        auto y_pixel_offset = (rc.max_clip_x * y);

        iz_ptr = rc.inv_z_buffer + y_pixel_offset;
        screen_buffer_ptr = rc.frame_buffer + y_pixel_offset;

        for(int x = x_start + 1; x < x_end; x++) {
            if (iz > iz_ptr[x]) {
                (screen_buffer_ptr + x)->value = poly.color.rgba_bit(i);

                iz_ptr[x] = iz;
            }

            i += di_dx;

            iz += diz_dx;
        }

        left.x += left.dx_dy;
        right.x += right.dx_dy;

        left.i += left.di_dy;
        right.i += right.di_dy;

        left.iz += left.diz_dy;
        right.iz += right.diz_dy;
    }
}


void draw_triangle_iinvzb(RenderListPoly &poly, RenderContext &rc) {
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

    auto bottom_to_top = IINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v2], rc};

    if (Math::f_cmp(poly.trans_verts[v0].v.y, poly.trans_verts[v1].v.y)) {
        auto bottom_to_middle = IINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        auto middle_to_top = IINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    } else {
        auto bottom_to_middle = IINVZBEdge{poly.trans_verts[v0], poly.trans_verts[v1], rc};
        auto middle_to_top = IINVZBEdge{poly.trans_verts[v1], poly.trans_verts[v2], rc};

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color, poly, rc);
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color, poly, rc);
    }
}

}

