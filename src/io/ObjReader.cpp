#include "ObjReader.h"

#include <sstream>

ObjReader::ObjReader() {  }

ObjReader::~ObjReader() {
    if (m_ifs.is_open()) {
        m_ifs.close();
    }
}

vector<Triangle> ObjReader::read_file(string path) {
    m_ifs = std::ifstream(path);

    std::vector<Triangle> triangles;

    if (!m_ifs.is_open())
        return triangles;

    std::vector<Vertex> vertexes;

    while(!m_ifs.eof()) {
        char line[128]; // presumption that lines aren't over 128 characters wide
        m_ifs.getline(line, 128);

        std::stringstream ss;
        ss << line;

        char temp;
        if (line[0] == 'v') {
            float x, y, z;
            ss >> temp >> x >> y >> z;

            vertexes.push_back(Vertex(x, y, z));
        }

        if (line[0] == 'f') {
            int f[3];
            ss >> temp >> f[0] >> f[1] >> f[2];
            triangles.push_back(Triangle(vertexes[f[0] -1], vertexes[f[1] - 1], vertexes[f[2] - 1]));
        }
    }

    return triangles;
}
