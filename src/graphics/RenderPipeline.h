#pragma once

#include <vector>

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "Rasterizer.h"
#include "RenderObject.h"
#include "Lighting.h"

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

void camera_trans_to_renderlist(RenderObject &object, const Matrix4x4 &vp, RenderContext &context);

void camera_transform_lights(const Matrix4x4 &vp);

void light_renderlist(RenderContext &context);

void backface_removal_object(RenderObject& object, const Camera &camera);

void frustrum_clip_renderlist(const Camera &camera, RenderContext &context);

void perspective_screen_transform_renderlist(const Camera &camera, RenderContext &context);

void draw_renderlist(RenderContext &context);

class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);
    void render_objects(const Camera &camera, std::vector<RenderObject> renderables, RenderContext &context);
private:
    Renderer* p_renderer = nullptr;
};

}

