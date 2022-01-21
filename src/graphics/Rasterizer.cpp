#include "Rasterizer.h"

Rasterizer::Rasterizer(Renderer* renderer) : p_renderer(renderer) {
}

Rasterizer::~Rasterizer() {
    delete[] p_z_buffer;
}

void Rasterizer::draw_triangle(Triangle &triangle, const Texture &texture) {
    bool v1_inside = triangle.p[0].inside_view_frustrum();
    bool v2_inside = triangle.p[1].inside_view_frustrum();
    bool v3_inside = triangle.p[2].inside_view_frustrum();

    if (v1_inside && v2_inside && v3_inside) {
        fill_triangle(triangle, texture);
    }

     std::vector<Vertex> vertices;
     std::vector<Vertex> auxilary_list;

     vertices.push_back(triangle.p[0]);
     vertices.push_back(triangle.p[1]);
     vertices.push_back(triangle.p[2]);

     // Add image space clipping
     if (clip_polygon_axis(vertices, auxilary_list, 0) &&
             clip_polygon_axis(vertices, auxilary_list, 1) &&
             clip_polygon_axis(vertices, auxilary_list, 2))
     {
         Vertex initial_vertex = vertices[0];

         for (int i = 1; i < static_cast<int>(vertices.size()) - 1; i++) {
             Triangle draw_triangle;
             draw_triangle.p[0] = initial_vertex;
             draw_triangle.p[1] = vertices[i];
             draw_triangle.p[2] = vertices[i + 1];

             fill_triangle(draw_triangle, texture);
         }
     }
}

void Rasterizer::fill_triangle(Triangle &triangle, const Texture &texture) {
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

	scan_triangle(min_y_vert, mid_y_vert, max_y_vert, handedness, texture);
}

void Rasterizer::clear_depth_buffer() {
    std::fill(p_z_buffer, p_z_buffer + m_width * m_height, INFINITY);
}

inline void Rasterizer::scan_triangle(const Vertex &min_y_vert, const Vertex &mid_y_vert, const Vertex &max_y_vert, bool handedness, const Texture &texture) {
    Gradients gradients = Gradients(min_y_vert, mid_y_vert, max_y_vert);

    Edge bottom_to_top = Edge(min_y_vert, max_y_vert, gradients, 0);
    Edge bottom_to_middle = Edge(min_y_vert, mid_y_vert, gradients, 0);
    Edge middle_to_top = Edge(mid_y_vert, max_y_vert, gradients, 1);

    scan_edges(bottom_to_top, bottom_to_middle, handedness, texture, gradients);
    scan_edges(bottom_to_top, middle_to_top, handedness, texture, gradients);
}

inline void Rasterizer::scan_edges(Edge &a, Edge &b, bool handedness, const Texture &texture, const Gradients &gradients) {
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

inline void Rasterizer::draw_scanline(const Edge &left, const Edge &right, int j, const Texture &texture, const Gradients &gradients) {
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

            if (src_x > 0 && src_y > 0) {
                uint32_t value = texture.get_pixel(src_x, src_y, light_amount).value;

                p_framebuffer[m_width * j + i].value = value;
                p_z_buffer[index] = depth;
            }

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
        p_framebuffer = p_renderer->get_framebuffer();
    }
}

inline bool Rasterizer::clip_polygon_axis(std::vector<Vertex> &vertices, std::vector<Vertex> &auxilary_list, int component_index) {
    clip_polygon_component(vertices, component_index, 1.0f, auxilary_list);
    vertices.clear();

    if(auxilary_list.empty())
    {
        return false;
    }

    clip_polygon_component(auxilary_list, component_index, -1.0f, vertices);
    auxilary_list.clear();

    return !vertices.empty();
}

inline void Rasterizer::clip_polygon_component(std::vector<Vertex> &vertices, int component_index,
    float component_factor, std::vector<Vertex> &result)
{
    Vertex prev_vertex = vertices[vertices.size() -1];
    float previous_component = prev_vertex.get(component_index) * component_factor;
    bool previous_inside = previous_component <= prev_vertex.pos.w;

    for (auto current_vertex : vertices) {
        float current_component = current_vertex.get(component_index) * component_factor;
        bool current_inside = current_component <= current_vertex.pos.w;

        if((current_inside || previous_inside) && current_inside != previous_inside)
        {
            float lerpAmt = (prev_vertex.pos.w - previous_component) /
                ((prev_vertex.pos.w - previous_component) -
                 (current_vertex.pos.w - current_component));

            result.push_back(prev_vertex.lerp(current_vertex, lerpAmt));
        }

        if(current_inside)
        {
            result.push_back(current_vertex);
        }

        prev_vertex = current_vertex;
        previous_component = current_component;
        previous_inside = current_inside;
    }
}
