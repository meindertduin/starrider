#pragma once

#include <string>
#include <vector>

#include "../graphics/Core.h"

using std::string;
using std::vector;

struct ObjIndex {
    int vertex_index;
    int tex_coord_index;
    int normal_index;
};

class ObjReader {
public:
    ObjReader();
    bool read_file(string path);
    vector<Triangle> create_vertices();
private:
    vector<V4F> m_vertices;
    vector<V4F> m_tex_coords;
    vector<ObjIndex> m_indices;
    vector<V4F> m_normals;

    bool has_tex_coords = false;
    bool has_normal_indices = false;

    ObjIndex parse_object_index(string token);
};
