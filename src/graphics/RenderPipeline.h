#pragma once

#include <vector>

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "Rasterizer.h"
#include "RenderObject.h"

namespace Graphics {

enum class CoordSelect {
    Local_To_Trans,
    Trans_Only,
};

constexpr void camera_transform(const RenderObject &renderable, const Matrix4x4 &vp, const Polygon &current_poly, V4D *points) {
    points[0] = vp.transform(renderable.transformed_vertices[current_poly.vert[0]].v);
    points[1] = vp.transform(renderable.transformed_vertices[current_poly.vert[1]].v);
    points[2] = vp.transform(renderable.transformed_vertices[current_poly.vert[2]].v);
}

void world_transform_object(RenderObject &object, CoordSelect coord_select = CoordSelect::Local_To_Trans);

void light_camera_transform_object(RenderObject &object, const Matrix4x4 &vp, std::vector<RenderPolygon> &render_list);

void backface_removal_object(RenderObject& object, const Camera &camera);

void perspective_screen_transform(const Camera &camera, V4D *points);

RGBA light_polygon(const Polygon &polygon, Light *lights, int max_lights);


class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);
    void render_objects(const Camera &camera, std::vector<RenderObject> renderables);
private:
    Renderer* p_renderer = nullptr;
};

}

