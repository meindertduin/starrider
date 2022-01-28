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

class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);

    void render_objects(const Camera &camera, std::vector<RenderObject> renderables);
private:
    Rasterizer m_rasterizer;

    Renderer* p_renderer = nullptr;
    void perspective_screen_transform(const Camera &camera, V4D *points);
    RGBA light_polygon(const Polygon &polygon, const Camera &camera, Light *lights, int max_lights);
    constexpr void camera_transform(const RenderObject &renderable, const Matrix4x4 &vp, const Polygon &current_poly, V4D *points) {
        points[0] = vp.transform(renderable.transformed_points[current_poly.vert[0]]);
        points[1] = vp.transform(renderable.transformed_points[current_poly.vert[1]]);
        points[2] = vp.transform(renderable.transformed_points[current_poly.vert[2]]);
    }

    void camera_transform(const RenderObject &renderable, const Matrix4x4 &vp, const Polygon &current_poly) {
    }
};
