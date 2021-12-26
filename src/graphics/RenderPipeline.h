#pragma once

#include <vector>

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "Rasterizer.h"

struct Renderable {
    Transform *transform;
    Mesh *mesh;
};

class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);

    void render_viewport(const Camera &camera, std::vector<Renderable> renderables);
private:
    Rasterizer m_rasterizer;

    Renderer* p_renderer = nullptr;
};
