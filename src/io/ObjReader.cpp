#include <sstream>
#include <fstream>
#include <map>

#include "ObjReader.h"

ObjReader::ObjReader() {

}

bool ObjReader::read_file(string path, int text_width, int text_height) {
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

            m_vertices.push_back(V4D(x, y, z));
        }

        if (first_token == "vt") {
            float x, y;
            ss >> x >> y;

            m_tex_coords.push_back(Math::V2D(x, y));
        }

        if (first_token == "vn") {
            float x, y, z;
            ss >> x >> y >> z;
            m_normals.push_back(V4D(x, y, z, 0)); // normals dont have positions so w == 0
        }

        if (first_token == "f") {
            vector<string> tokens;
            string value;

            while(std::getline(ss, value, ' '))
                tokens.push_back(value);

            for (int i = 0; i < tokens.size() - 3; i++) {
                m_indices.push_back(parse_object_index(tokens[1]));
                m_indices.push_back(parse_object_index(tokens[2 + i]));
                m_indices.push_back(parse_object_index(tokens[3 + i]));
            }
        }
    }

    fs.close();
    return true;
}

void ObjReader::create_render_object(RenderObject &object, Texture *texture) {
    object.state = ObjectStateActive | ObjectStateVisible;

    // TODO: read if object has multiple frames out of the file
    object.frames_count = 1;
    object.curr_frame = 0;
    object.attributes |= ObjectAttributeSingleFrame;

    object.vertex_count = m_vertices.size();
    object.text_count = m_tex_coords.size();

    object.local_vertices = new Vertex4D[object.vertex_count];
    object.transformed_vertices = new Vertex4D[object.vertex_count];
    object.texture_coords = new Point2D[m_tex_coords.size()];

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = RGBA(255, 255, 255, 255);

    for (int i = 0; i < object.vertex_count; i++) {
        object.local_vertices[i].v = m_vertices[i];
        object.local_vertices[i].attributes = VertexAttributePoint;
    }

    for (int i = 0; i < object.text_count; i++) {
        object.texture_coords[i] = m_tex_coords[i];
    }

    std::vector<Polygon> polygons;
    for (int i = 0; i < m_indices.size(); i += 3) {
        Polygon polygon;
        polygon.texture = texture;
        polygon.vertices = object.local_vertices;
        polygon.text_coords = object.texture_coords;

        // TODO: get these values from obj file
        polygon.state = PolyStateActive;

        polygon.attributes = PolyAttributeTwoSided | PolyAttributeRGB24 |
            PolyAttributeShadeModeIntensityGourad;

        if (has_tex_coords) {
            polygon.attributes |= PolyAttributeShadeModeTexture;
        }

        for (int j = 0; j < 3; j++) {
            auto current_index = m_indices[i + j];
            polygon.vert[j] = current_index.vertex_index;

            if (has_tex_coords) {
                polygon.vertices[current_index.vertex_index].t = object.texture_coords[current_index.tex_coord_index];
                polygon.text[j] = current_index.tex_coord_index;

                polygon.vertices[current_index.vertex_index].attributes |= VertexAttributeTexture;
            }

            if (has_normal_indices) {
                polygon.vertices[current_index.vertex_index].n = m_normals[current_index.normal_index];
                polygon.vertices[current_index.vertex_index].attributes |= VertexAttributeNormal;
            }
        }

        if (polygon.attributes & PolyAttributeShadeModeGouraud ||
                polygon.attributes & PolyAttributeShadeModeIntensityGourad) {

            auto line1 = object.transformed_vertices[polygon.vert[0]].v
                - object.transformed_vertices[polygon.vert[1]].v;

            auto line2 = object.transformed_vertices[polygon.vert[0]].v
                - object.transformed_vertices[polygon.vert[2]].v;

            polygon.n_length = line1.cross(line2).length();
            polygon.text_coords = object.texture_coords;
        }

        // TODO read color out of file
        polygon.color = object.color;
        polygons.push_back(polygon);
    }


    object.poly_count = polygons.size();
    object.polygons = new Polygon[object.poly_count];

    for (int i = 0; i < polygons.size(); i++) {
        object.polygons[i] = polygons[i];
    }

    if (!has_normal_indices) {
        compute_vertex_normals(object);
    }
}

int ObjReader::compute_vertex_normals(RenderObject &object) {
    int polys_touch_vertices[ObjectMaxVertices];
    memset((void*)polys_touch_vertices, 0, sizeof(int) * ObjectMaxVertices);

    for (int poly = 0; poly < object.poly_count; poly++) {
        if (object.polygons[poly].attributes & PolyAttributeShadeModeGouraud) {
            int vi0 = object.polygons[poly].vert[0];
            int vi1 = object.polygons[poly].vert[1];
            int vi2 = object.polygons[poly].vert[2];

            auto line1 = object.local_vertices[vi0].v
                - object.local_vertices[vi1].v;

            auto line2 = object.local_vertices[vi0].v
                - object.local_vertices[vi2].v;

            auto n = line1.cross(line2);

            object.polygons[poly].n_length = n.length();

            polys_touch_vertices[vi0]++;
            polys_touch_vertices[vi1]++;
            polys_touch_vertices[vi2]++;

            object.local_vertices[vi0].n += n;
            object.local_vertices[vi1].n += n;
            object.local_vertices[vi2].n += n;
        }
    }

    for (int vertex = 0; vertex < object.vertex_count; vertex++) {
        if (polys_touch_vertices[vertex] >= 1) {
            object.local_vertices[vertex].n /= polys_touch_vertices[vertex];
            object.local_vertices[vertex].n.normalise();
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

