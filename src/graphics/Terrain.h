#include "RenderObject.h"
#include "../math/Parametric.h"

namespace Graphics {

struct TerrainTile {
    int x_pos;
    int y_pos;

    std::vector<int> polygon_indices;
};

struct TerrainObject {
    int grid_width;
    int grid_height;

    Math::Box bounding_box;

    StaticRenderObject object;

    std::vector<TerrainTile> tiles;
};

}
