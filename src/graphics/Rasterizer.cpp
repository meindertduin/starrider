#include "Rasterizer.h"
#include "../math/Core.h"

using Math::V2D;

namespace Graphics {

Pixel* p_frame_buffer;

int min_clip_y {0};
int min_clip_x {0};

int m_width {0};
int m_height {0};


void draw_gouraud_triangle(RenderListPoly &poly) {
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
        Edge bottom_to_top = Edge(poly.trans_verts[v0], poly.trans_verts[v2]);
        Edge bottom_to_middle = Edge(poly.trans_verts[v0], poly.trans_verts[v1]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color.to_argb_bit());
    }
    else if (Math::f_cmp(poly.trans_verts[v1].v.y, poly.trans_verts[v2].v.y)) {
        Edge bottom_to_top = Edge(poly.trans_verts[v0], poly.trans_verts[v2]);
        Edge middle_to_top = Edge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color.to_argb_bit());
    } else {
        Edge bottom_to_top = Edge(poly.trans_verts[v0], poly.trans_verts[v2]);
        Edge bottom_to_middle = Edge(poly.trans_verts[v0], poly.trans_verts[v1]);
        Edge middle_to_top = Edge(poly.trans_verts[v1], poly.trans_verts[v2]);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, poly.color.to_argb_bit());
        scan_edges(bottom_to_top, middle_to_top, handedness, poly.color.to_argb_bit());
    }
}

void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t color) {
    if ((Math::f_cmp(x1, x2) && Math::f_cmp(x2, x3)) || (Math::f_cmp(y1, y2) && Math::f_cmp(y2, y3)))
        return;

    float x_temp, y_temp;

    if (y2 < y1) {
        x_temp = x1;
        y_temp = y1;

        x1 = x2;
        y1 = y2;
        x2 = x_temp;
        y2 = y_temp;
    }

    if (y3 < y1) {
        x_temp = x1;
        y_temp = y1;

        x1 = x3;
        y1 = y3;
        x3 = x_temp;
        y3 = y_temp;
    }

    if (y3 < y2) {
        x_temp = x2;
        y_temp = y2;

        x2 = x3;
        y2 = y3;
        x3 = x_temp;
        y3 = y_temp;
    }

    if (y3 < min_clip_y || y1 > m_height ||
        (x1 < min_clip_x && x2 < min_clip_x && x3 < min_clip_x) ||
        (x1 > m_width && x2 > m_width && x3 > m_width))
    {
        return;
    }

    float dx1 = x3 - x1;
    float dy1 = y3 - y1;

    float dx2 = x2 - x1;
    float dy2 = y2 - y1;

    bool handedness =  (dx1 * dy2 - dx2 * dy1) >= 0.0f;

    if (Math::f_cmp(y1, y2)) {
        Edge bottom_to_top = Edge(x1, y1, x3, y3);
        Edge bottom_to_middle = Edge(x1, y1, x2, y2);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, color);
    }
    else if (Math::f_cmp(y2, y3)) {
        Edge bottom_to_top = Edge(x1, y1, x3, y3);
        Edge middle_to_top = Edge(x2, y2, x3, y3);

        scan_edges(bottom_to_top, middle_to_top, handedness, color);
    } else {
        Edge bottom_to_top = Edge(x1, y1, x3, y3);
        Edge bottom_to_middle = Edge(x1, y1, x2, y2);
        Edge middle_to_top = Edge(x2, y2, x3, y3);

        scan_edges(bottom_to_top, bottom_to_middle, handedness, color);
        scan_edges(bottom_to_top, middle_to_top, handedness, color);
    }
}

void scan_edges(Edge &long_edge, Edge &short_edge, bool handedness, uint32_t color) {
    int y_start = short_edge.y_start;
    int y_end = short_edge.y_end;

    Edge &left = handedness ? short_edge : long_edge;
    Edge &right = handedness ? long_edge : short_edge;

    for(int y = y_start; y < y_end; y++) {
        for(int x = left.x; x < right.x; x++) {
            if (x > 0 && y > 0 && x < m_width && y < m_height)
                p_frame_buffer[m_width * y + x].value = color;
        }
        long_edge.x += long_edge.x_step;
        short_edge.x += short_edge.x_step;
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

