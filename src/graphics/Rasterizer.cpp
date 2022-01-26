#include "Rasterizer.h"

using Math::V2D;

Rasterizer::Rasterizer(Renderer* renderer) : p_renderer(renderer) {
}

Rasterizer::~Rasterizer() {
    delete[] p_z_buffer;
}

void Rasterizer::draw_triangle(V2D points[3]) {
    V2D min_y_vert = points[0];
	V2D mid_y_vert = points[1];
	V2D max_y_vert = points[2];

	if(max_y_vert.y < mid_y_vert.y)
	{
        V2D temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

	if(mid_y_vert.y < min_y_vert.y)
	{
		V2D temp = mid_y_vert;
		mid_y_vert = min_y_vert;
		min_y_vert = temp;
	}

	if(max_y_vert.y < mid_y_vert.y)
	{
		V2D temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

    float x1 = max_y_vert.x - min_y_vert.x;
    float y1 = max_y_vert.y - min_y_vert.y;

    float x2 = mid_y_vert.x - min_y_vert.x;
    float y2 = mid_y_vert.y - min_y_vert.y;

    bool handedness =  (x1 * y2 - x2 * y1) >= 0.0f;

    Edge bottom_to_top = Edge(min_y_vert, max_y_vert);
    Edge bottom_to_middle = Edge(min_y_vert, mid_y_vert);
    Edge middle_to_top = Edge(mid_y_vert, max_y_vert);

    scan_edges(bottom_to_top, bottom_to_middle, handedness);
    scan_edges(bottom_to_top, middle_to_top, handedness);
}

void Rasterizer::clear_depth_buffer() {
    std::fill(p_z_buffer, p_z_buffer + m_width * m_height, INFINITY);
}

inline void Rasterizer::scan_edges(Edge &a, Edge &b, bool handedness) {
    int y_start = b.y_start;
    int y_end = b.y_end;

    for(int j = y_start; j < y_end; j++) {
        if (handedness) {
            draw_scanline(b, a, j);
        } else {
            draw_scanline(a, b, j);
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
