#pragma once

#include <vector>

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "Rasterizer.h"
#include "RenderObject.h"

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
};
