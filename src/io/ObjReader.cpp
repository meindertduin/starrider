#include <sstream>
#include <fstream>
#include <map>

#include "ObjReader.h"

ObjReader::ObjReader() {

}

bool ObjReader::read_file(string path) {
    std::ifstream fs(path);

    m_vertices.clear();
    m_tex_coords.clear();
    m_indices.clear();
    m_normals.clear();

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

            // Wavefront .obj files output the uv coordinate system with the y-axis starting at the bottom.
            // This engine however expects y = 0 to start at the top. So the y values of textures have to be inversed.
            m_tex_coords.push_back(Math::V2D(x, 1.0f - y));
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

