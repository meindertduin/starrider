
#include "Rasterizer.h"

Rasterizer::Rasterizer(Renderer* renderer) : p_renderer(renderer) {

}

Rasterizer::~Rasterizer() {
    delete[] p_z_buffer;
}

void Rasterizer::draw_triangle(const Triangle &triangle, const Bitmap &texture) {
    Vertex min_y_vert = triangle.p[0];
	Vertex mid_y_vert = triangle.p[1];
	Vertex max_y_vert = triangle.p[2];

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

	scan_triangle(min_y_vert, mid_y_vert, max_y_vert, handedness, texture);
}

void Rasterizer::clear_depth_buffer() {
    for (int i = 0; i < m_width * m_height; i++) {
        p_z_buffer[i] = INFINITY;
    }
}

void Rasterizer::scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Bitmap &texture) {
    Gradients gradients = Gradients(min_y_vert, mid_y_vert, max_y_vert);

    Edge bottom_to_top = Edge(min_y_vert, max_y_vert, gradients, 0);
    Edge bottom_to_middle = Edge(min_y_vert, mid_y_vert, gradients, 0);
    Edge middle_to_top = Edge(mid_y_vert, max_y_vert, gradients, 1);

    scan_edges(bottom_to_top, bottom_to_middle, handedness, texture, gradients);
    scan_edges(bottom_to_top, middle_to_top, handedness, texture, gradients);
}

void Rasterizer::scan_edges(Edge &a, Edge &b, bool handedness, const Bitmap &texture, const Gradients &gradients) {
    int y_start = b.y_start;
    int y_end   = b.y_end;

    for(int j = y_start; j < y_end; j++) {
        if (handedness) {
            draw_scanline(b, a, j, texture, gradients);
        } else {
            draw_scanline(a, b, j, texture, gradients);
        }
        a.step();
        b.step();
    }
}

void Rasterizer::draw_scanline(const Edge &left, const Edge &right, int j, const Bitmap &texture, const Gradients &gradients) {
    int x_min = (int)std::ceil(left.x);
    int x_max = (int)std::ceil(right.x);
    float x_prestep = (float)x_min - left.x;

    float x_dist = right.x - left.x;
    float text_coord_x_xstep = gradients.text_coord_x_xstep;
    float text_coord_y_xstep = gradients.text_coord_y_xstep;
    float one_over_zx_step = gradients.one_over_zx_step;
    float depth_x_step = gradients.depth_x_step;
    float light_amount_xstep = gradients.light_amount_xstep;

    float text_coord_x = left.text_coord_x + text_coord_x_xstep * x_prestep;
    float text_coord_y = left.text_coord_y + text_coord_x_xstep * x_prestep;
    float one_over_z = left.one_over_z + one_over_zx_step * x_prestep;
    float depth = left.depth - depth_x_step * x_prestep;
    float light_amount = left.light_amount + gradients.light_amount_xstep * x_prestep;

    for(int i = x_min; i < x_max; i++)
    {
        int index = i + j * m_width;
        if (p_z_buffer[index] > depth) {
            float z = 1.0f / one_over_z;
            int src_x = (int)((text_coord_x * z) * (texture.width - 1) + 0.5f);
            int src_y = (int)((text_coord_y * z) * (texture.height - 1) + 0.5f);

            uint32_t value = texture.get_value(src_x, src_y, light_amount);

            p_renderer->set_frame_pixel(i, j, value);
            p_z_buffer[index] = depth;
        }

        text_coord_x += text_coord_x_xstep;
        text_coord_y += text_coord_y_xstep;
        one_over_z += one_over_zx_step;
        depth += depth_x_step;
        light_amount += light_amount_xstep;
    }
}

void Rasterizer::set_viewport(int width, int height) {
    if (m_width != width || height != m_height) {
        m_width = width;
        m_height = height;

        delete[] p_z_buffer;
        p_z_buffer = new float[width * height];
    }
}
