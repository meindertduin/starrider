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

constexpr void camera_transform(const RenderObject &renderable, const Matrix4x4 &vp, const Polygon &current_poly, RenderListPoly &list_poly) {
    list_poly.trans_verts[0].v = vp.transform(renderable.transformed_vertices[current_poly.vert[0]].v);
    list_poly.trans_verts[1].v = vp.transform(renderable.transformed_vertices[current_poly.vert[1]].v);
    list_poly.trans_verts[2].v = vp.transform(renderable.transformed_vertices[current_poly.vert[2]].v);

    list_poly.trans_verts[0].n = vp.transform(renderable.transformed_vertices[current_poly.vert[0]].n);
    list_poly.trans_verts[1].n = vp.transform(renderable.transformed_vertices[current_poly.vert[1]].n);
    list_poly.trans_verts[2].n = vp.transform(renderable.transformed_vertices[current_poly.vert[2]].n);
}

void world_transform_object(RenderObject &object, CoordSelect coord_select = CoordSelect::Local_To_Trans);

void light_camera_transform_object(RenderObject &object, const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list);

void backface_removal_object(RenderObject& object, const Camera &camera);

void perspective_screen_transform(const Camera &camera, RenderListPoly &poly);

void light_polygon(Polygon &polygon, Light *lights, int max_lights);

void flat_light_polygon(Polygon &polygon, Light *lights, int max_lights);


class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);
    void render_objects(const Camera &camera, std::vector<RenderObject> renderables);
private:
    Renderer* p_renderer = nullptr;
};

}

