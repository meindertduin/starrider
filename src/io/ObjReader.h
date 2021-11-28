#pragma once

#include <string>
#include <fstream>
#include <vector>

using std::string;
using std::vector;

struct Vertex {
    Vertex(float x, float y, float z);
};

struct Triangle {
    Triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3);
};


class ObjReader {
public:
    ObjReader();
    ~ObjReader();
    vector<Triangle> read_file(string path);
private:
    std::ifstream m_ifs;
};
