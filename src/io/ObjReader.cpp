#include <sstream>
#include <fstream>

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

            m_vertices.push_back(V4F(x, y, z));
        }

        if (first_token == "vt") {
            float x, y;
            ss >> x >> y;

            m_tex_coords.push_back(std::pair<float, float>(x, y));
        }

        if (first_token == "vn") {
            float x, y, z;
            ss >> x >> y >> z;
            m_normals.push_back(V4F(x, y, z));
        }

        if (first_token == "f") {
            vector<string> tokens;
            string value;

            while(std::getline(ss, value, ' '))
                tokens.push_back(value);

            for (int i = 0; i < tokens.size() - 2; i++) {
                m_indices.push_back(parse_object_index(tokens[0]));
                m_indices.push_back(parse_object_index(tokens[1 + i]));
                m_indices.push_back(parse_object_index(tokens[2 + i]));
            }
        }
    }

    fs.close();
    return true;
}

vector<Vertex> ObjReader::create_vertices() {
    vector<Vertex> result;
}

ObjIndex ObjReader::parse_object_index(string token) {
    ObjIndex result;
    std::stringstream ss;
    ss << token;

    int i = 0;
    string value;

    while(std::getline(ss, value, '/')) {
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
