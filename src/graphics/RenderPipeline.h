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

    void render_frame(const Camera &camera, std::vector<Renderable> renderables);
private:
    Renderer* p_renderer;
    Rasterizer m_rasterizer;
};
