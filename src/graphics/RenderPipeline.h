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

constexpr void camera_transform(const Matrix4x4 &vp, RenderListPoly &list_poly) {
    list_poly.trans_verts[0].v = vp.transform(list_poly.trans_verts[0].v);
    list_poly.trans_verts[1].v = vp.transform(list_poly.trans_verts[1].v);
    list_poly.trans_verts[2].v = vp.transform(list_poly.trans_verts[2].v);
}

void world_transform_object(RenderObject &object, CoordSelect coord_select = CoordSelect::Local_To_Trans);

void light_camera_transform_object(RenderObject &object, const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list);

void backface_removal_object(RenderObject& object, const Camera &camera);

void perspective_screen_transform(const Camera &camera, RenderListPoly &poly);

void gourad_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights);

void gourad_intensity_light_polygon(RenderListPoly &polygon, Light *lights, int max_lights);

void flat_light_polygon(Polygon &polygon, Light *lights, int max_lights);


class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);
    void render_objects(const Camera &camera, std::vector<RenderObject> renderables);
private:
    Renderer* p_renderer = nullptr;
};

}

