#pragma once

#include <vector>

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "Rasterizer.h"
#include "RenderObject.h"

enum class CoordSelect {
    Local_To_Trans,
    Trans_Only,
};

struct Renderable {
    Transform *transform;
    Mesh *mesh;
};

class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);

    void render_objects(const Camera &camera, std::vector<RenderObject> renderables);
private:
    Rasterizer m_rasterizer;

    Renderer* p_renderer = nullptr;
    void transform_world_pos(RenderObject &object, CoordSelect coord_select = CoordSelect::Local_To_Trans);
};
