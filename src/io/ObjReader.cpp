#include <sstream>
#include <fstream>
#include <map>

#include "ObjReader.h"

ObjReader::ObjReader() {

}

bool ObjReader::read_file(string path) {
    std::ifstream fs(path);

    if (!fs.is_open())
        return false;

    while(!fs.eof()) {
        char line[128];

        fs.getline(line, sizeof(line));

        std::stringstream ss;
        vector<string> tokens;
        ss << line;

        string first_token;
        ss >> first_token;

        if (first_token == "v") {
            float x, y, z;
            ss >> x >> y >> z;

            // Wavefront .obj files are exported for a right handed coordinate system.
            // Thats why the x-axis is inversed for our left handed coordinate engine.
            m_vertices.push_back(V4D(x * -1.0f, y, z));
        }

        if (first_token == "vt") {
            float x, y;
            ss >> x >> y;

            m_tex_coords.push_back(Math::V2D(x, y));
        }

        if (first_token == "vn") {
            float x, y, z;
            ss >> x >> y >> z;

            // Wavefront .obj files are exported for a right handed coordinate system.
            // Thats why the normal x-axis is inversed for our left handed coordinate engine.
            m_normals.push_back(V4D(x * -1.0f, y, z, 0)); // normals dont have positions so w == 0
        }

        if (first_token == "f") {
            vector<string> tokens;
            string value;

            while(std::getline(ss, value, ' '))
                tokens.push_back(value);

            for (int i = 0; i < tokens.size() - 3; i++) {
                // Wavefront .obj files are exported for a right handed coordinate system.
                // Therefore we load load the faces in reverse order to have the face to the outside rather than the inside.
                m_indices.push_back(parse_object_index(tokens[3 + i]));
                m_indices.push_back(parse_object_index(tokens[2 + i]));
                m_indices.push_back(parse_object_index(tokens[1]));
            }
        }
    }

    fs.close();
    return true;
}

void ObjReader::extract_content(Graphics::Mesh &result, Graphics::MeshAttributes attributes) {
    result.vertex_count = m_vertices.size();
    result.text_count = m_tex_coords.size();

    result.vertices = new Graphics::Vertex4D[result.vertex_count];
    result.text_coords = new Graphics::Point2D[m_tex_coords.size()];

    for (int i = 0; i < result.vertex_count; i++) {
        result.vertices[i].v = m_vertices[i];
        result.vertices[i].attributes = Graphics::VertexAttributePoint;
    }

    std::copy(m_tex_coords.begin(), m_tex_coords.end(), result.text_coords);

    std::vector<Graphics::Polygon> polygons;
    for (int i = 0; i < m_indices.size(); i += 3) {
        Graphics::Polygon polygon;
        polygon.vertices = result.vertices;
        polygon.text_coords = result.text_coords;

        polygon.state = attributes.poly_state;

        polygon.attributes = attributes.poly_attributes;

        for (int j = 0; j < 3; j++) {
            auto current_index = m_indices[i + j];
            polygon.vert[j] = current_index.vertex_index;

            if (has_tex_coords) {
                polygon.vertices[current_index.vertex_index].t = result.text_coords[current_index.tex_coord_index];

                polygon.text[j] = current_index.tex_coord_index;

                polygon.vertices[current_index.vertex_index].attributes |= Graphics::VertexAttributeTexture;
            }

            if (has_normal_indices) {
                polygon.vertices[current_index.vertex_index].n = m_normals[current_index.normal_index];
                polygon.vertices[current_index.vertex_index].attributes |= Graphics::VertexAttributeNormal;
            }
        }

        if (polygon.attributes & Graphics::PolyAttributeShadeModeGouraud ||
                polygon.attributes & Graphics::PolyAttributeShadeModeIntensityGourad) {

            auto line1 = result.vertices[polygon.vert[0]].v
                - result.vertices[polygon.vert[1]].v;

            auto line2 = result.vertices[polygon.vert[0]].v
                - result.vertices[polygon.vert[2]].v;

            polygon.n_length = line1.cross(line2).length();
            polygon.text_coords = result.text_coords;
        }

        polygon.color = attributes.poly_color;

        polygons.push_back(polygon);
    }

    result.polygons = polygons;

    // Wavefront .obj files output the uv coordinate system with the y-axis starting at the bottom.
    // This engine however expects y = 0 to start at the top. So the y values of textures have to be inversed.
    for (int i = 0; i < result.text_count; i++) {
        result.text_coords[i].y = 1.0f - result.text_coords[i].y;
    }

    if (!has_normal_indices) {
        compute_vertex_normals(result);
    }
}

int ObjReader::compute_vertex_normals(Graphics::Mesh &object) {
    int polys_touch_vertices[Graphics::ObjectMaxVertices];
    memset((void*)polys_touch_vertices, 0, sizeof(int) * Graphics::ObjectMaxVertices);

    for (int poly = 0; poly < object.polygons.size(); poly++) {
        if (object.polygons[poly].attributes & Graphics::PolyAttributeShadeModeGouraud) {
            int vi0 = object.polygons[poly].vert[0];
            int vi1 = object.polygons[poly].vert[1];
            int vi2 = object.polygons[poly].vert[2];

            auto line1 = object.vertices[vi0].v
                - object.vertices[vi1].v;

            auto line2 = object.vertices[vi0].v
                - object.vertices[vi2].v;

            auto n = line1.cross(line2);

            object.polygons[poly].n_length = n.length();

            polys_touch_vertices[vi0]++;
            polys_touch_vertices[vi1]++;
            polys_touch_vertices[vi2]++;

            object.vertices[vi0].n += n;
            object.vertices[vi1].n += n;
            object.vertices[vi2].n += n;
        }
    }

    for (int vertex = 0; vertex < object.vertex_count; vertex++) {
        if (polys_touch_vertices[vertex] >= 1) {
            object.vertices[vertex].n /= polys_touch_vertices[vertex];
            object.vertices[vertex].n.normalise();
        }
    }

    return 1;
}

ObjIndex ObjReader::parse_object_index(string token) {
    ObjIndex result;
    std::stringstream ss;
    ss << token;

    int i = 0;
    string value;

    while(std::getline(ss, value, '/')) {
        if (value == "") {
            i++;
            continue;
        }

        if (i == 0) {
            result.vertex_index = std::stoi(value) - 1;
        } else if (i == 1) {
            result.tex_coord_index = std::stoi(value) - 1;
            has_tex_coords = true;
        } else if(i == 2) {
            result.normal_index = std::stoi(value) - 1;
            has_normal_indices = true;
            break;
        }

        i++;
    }

    return result;
}

