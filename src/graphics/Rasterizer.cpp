#include "Rasterizer.h"

Rasterizer::Rasterizer(Renderer* renderer) : p_renderer(renderer) {
}

Rasterizer::~Rasterizer() {
    delete[] p_z_buffer;
}

void Rasterizer::draw_triangle(Triangle &triangle, const Texture &texture) {
    auto screen_space = Math::mat_4x4_screen_space((float)m_width / 2.0f, (float)m_height / 2.0f);
    auto identity = Math::Mat_4x4_Identity;

    Vertex min_y_vert = triangle.p[0].transform(screen_space, identity).perspective_divide();
	Vertex mid_y_vert = triangle.p[1].transform(screen_space, identity).perspective_divide();
	Vertex max_y_vert = triangle.p[2].transform(screen_space, identity).perspective_divide();

	if(max_y_vert.pos.y < mid_y_vert.pos.y)
	{
		Vertex temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

	if(mid_y_vert.pos.y < min_y_vert.pos.y)
	{
		Vertex temp = mid_y_vert;
		mid_y_vert = min_y_vert;
		min_y_vert = temp;
	}

	if(max_y_vert.pos.y < mid_y_vert.pos.y)
	{
		Vertex temp = max_y_vert;
		max_y_vert = mid_y_vert;
		mid_y_vert = temp;
	}

    float x1 = max_y_vert.pos.x - min_y_vert.pos.x;
    float y1 = max_y_vert.pos.y - min_y_vert.pos.y;

    float x2 = mid_y_vert.pos.x - min_y_vert.pos.x;
    float y2 = mid_y_vert.pos.y - min_y_vert.pos.y;

    bool handedness =  (x1 * y2 - x2 * y1) >= 0.0f;

    Edge bottom_to_top = Edge(min_y_vert, max_y_vert, 0);
    Edge bottom_to_middle = Edge(min_y_vert, mid_y_vert, 0);
    Edge middle_to_top = Edge(mid_y_vert, max_y_vert, 1);

    scan_edges(bottom_to_top, bottom_to_middle, handedness, texture);
    scan_edges(bottom_to_top, middle_to_top, handedness, texture);
}

void Rasterizer::clear_depth_buffer() {
    std::fill(p_z_buffer, p_z_buffer + m_width * m_height, INFINITY);
}

inline void Rasterizer::scan_edges(Edge &a, Edge &b, bool handedness, const Texture &texture) {
    int y_start = b.y_start;
    int y_end = b.y_end;

    for(int j = y_start; j < y_end; j++) {
        if (handedness) {
            draw_scanline(b, a, j, texture);
        } else {
            draw_scanline(a, b, j, texture);
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
