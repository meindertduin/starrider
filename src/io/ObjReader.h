#pragma once

#include <string>
#include <vector>

#include "../graphics/Core.h"
#include "../graphics/RenderObject.h"
#include "../math/Vector.h"

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
    void extract_content(Geometry &result);
    void create_render_object(RenderObject &object, Texture *texture);
private:
    vector<V4D> m_vertices;
    vector<Math::V2D> m_tex_coords;
    vector<ObjIndex> m_indices;
    vector<V4D> m_normals;

    bool has_tex_coords = false;
    bool has_normal_indices = false;

    ObjIndex parse_object_index(string token);
    int compute_vertex_normals(Geometry &object);
};

