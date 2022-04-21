#include "RenderObject.h"

namespace Graphics {

Material_Type g_materials[MaxMaterials];

void reset_materials() {
    static int first_time = 1;
    if (first_time) {
        std::memset(g_materials, 0, MaxMaterials * sizeof(Material_Type));
        first_time = 0;
    }

    for (int curr_mat = 0; curr_mat < MaxMaterials; curr_mat++) {
        delete g_materials[curr_mat].texutre;
        std::memset(&g_materials[curr_mat], 0, sizeof(Material_Type));
    }
}

int RenderObject_Type::set_frame(int frame) {
    if (!(this->attributes & ObjectAttributeMultiFrame))
        return 0;

    if (frame < 0) {
        frame = 0;
    }

    this->curr_frame = frame;

    this->local_vertices = &(this->head_local_vertices[frame * this->vertex_count]);
    this->transformed_vertices = &(this->head_transformed_vertices[frame * this->vertex_count]);

    return 1;
}

void RenderObject::next_frame() {
    if (curr_frame < frames_count) {
        this->set_frame(curr_frame + 1);
        curr_frame++;
        return;
    }

    this->set_frame(0);
    curr_frame = 0;
}

}
