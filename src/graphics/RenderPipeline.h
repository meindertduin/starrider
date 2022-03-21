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

constexpr const uint32_t RCAttributeNoBuffer =          1 << 0;
constexpr const uint32_t RCAttributeZSort =             1 << 1;
constexpr const uint32_t RCAttributeINVZBuffer =        1 << 2;
constexpr const uint32_t RCAttributeMipMapped =         1 << 3;
constexpr const uint32_t RCAttributeAlhpa =             1 << 4;

constexpr const uint32_t RCAttributeTextureAffine =     1 << 5;
constexpr const uint32_t RCAttributeTextureCorrect =    1 << 6;
constexpr const uint32_t RCAttributeTexturePiecewise =  1 << 7;
constexpr const uint32_t RCAttributeTextureHybrid =     1 << 8;


struct RenderContext {
    int attributes;
    int mip_z_dist;
    std::vector<RenderObject> renderables;
    float perfect_dist;
    float piecewise_dist;
};

constexpr void camera_transform(const Matrix4x4 &vp, RenderListPoly &list_poly) {
    list_poly.trans_verts[0].v = vp.transform(list_poly.trans_verts[0].v);
    list_poly.trans_verts[1].v = vp.transform(list_poly.trans_verts[1].v);
    list_poly.trans_verts[2].v = vp.transform(list_poly.trans_verts[2].v);
}

void world_transform_object(RenderObject &object, CoordSelect coord_select = CoordSelect::Local_To_Trans);

void camera_transform_renderlist(const Matrix4x4 &vp, std::vector<RenderListPoly> &render_list);

void camera_transform_lights(const Matrix4x4 &vp);

void light_renderlist(std::vector<RenderListPoly> &render_list);

void backface_removal_object(RenderObject& object, const Camera &camera);

void frustrum_clip_renderlist(const Camera &camera, std::vector<RenderListPoly> &render_list);

void perspective_screen_transform(const Camera &camera, RenderListPoly &poly);

void camera_trans_to_renderlist(RenderObject &object, std::vector<RenderListPoly> &render_list, const Matrix4x4 &vp, RenderContext &context);

class RenderPipeline {
public:
    RenderPipeline(Renderer *renderer);
    void render_objects(const Camera &camera, RenderContext &context);
private:
    Renderer* p_renderer = nullptr;
};

}

