#include "Rasterizer.h"

using Math::V2D;

Rasterizer::Rasterizer(Renderer* renderer) : p_renderer(renderer) {
}

Rasterizer::~Rasterizer() {
    delete[] p_z_buffer;
}

void Rasterizer::draw_triangle(V4D points[3], RGBA color) {
    // Points[0] is min_y_vert, points[1] is mid_y_vert, and points[2] max_y_vert
	if(points[2].y < points[1].y)
	{
        V4D temp = points[2];
		points[2] = points[1];
		points[1] = temp;
	}

	if(points[1].y < points[0].y)
	{
		V4D temp = points[1];
		points[1] = points[0];
		points[0] = temp;
	}

	if(points[2].y < points[1].y)
	{
		V4D temp = points[2];
		points[2] = points[1];
		points[1] = temp;
	}

    float x1 = points[2].x - points[0].x;
    float y1 = points[2].y - points[0].y;

    float x2 = points[1].x - points[0].x;
    float y2 = points[1].y - points[0].y;

    bool handedness =  (x1 * y2 - x2 * y1) >= 0.0f;

    Edge bottom_to_top = Edge(points[0], points[2]);
    Edge bottom_to_middle = Edge(points[0], points[1]);
    Edge middle_to_top = Edge(points[1], points[2]);

    scan_edges(bottom_to_top, bottom_to_middle, handedness, color);
    scan_edges(bottom_to_top, middle_to_top, handedness, color);
}

void Rasterizer::clear_depth_buffer() {
    std::fill(p_z_buffer, p_z_buffer + m_width * m_height, INFINITY);
}

inline void Rasterizer::scan_edges(Edge &a, Edge &b, bool handedness, RGBA color) {
    int y_start = b.y_start;
    int y_end = b.y_end;

    for(int y = y_start; y < y_end; y++) {
        if (handedness) {
            draw_scanline(b, a, y, color);
        } else {
            draw_scanline(a, b, y, color);
        }
        a.step();
        b.step();
    }
}

void Rasterizer::set_viewport(int width, int height) {
    if (m_width != width || height != m_height) {
        m_width = width;
        m_height = height;

        delete[] p_z_buffer;

        p_z_buffer = new float[width * height];
        p_framebuffer = p_renderer->get_framebuffer();
    }
}
