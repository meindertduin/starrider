#include "Core.h"
#include <sstream>
#include <fstream>

Vertex Vertex::transform(const Matrix4F &m) {
    return Vertex(m.transform(pos), text_coords);
}

bool Mesh::load_from_obj_file(std::string path) {
    std::ifstream fs(path);

    if (!fs.is_open())
        return false;

    std::vector<Vertex> vertexes;

    while(!fs.eof()) {
        char line[128]; // presumption that lines aren't over 128 characters wide
        fs.getline(line, 128);

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

    fs.close();
    return true;
}
