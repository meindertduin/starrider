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
    void perspective_screen_transform(Triangle &proj_tri, const Camera &camera);
    constexpr Triangle camera_transform(const RenderObject &renderable, const Polygon &current_poly, const Matrix4x4 vp) {
        return Triangle {
            Vertex {
                vp.transform(renderable.transformed_points[current_poly.vert[0]]),
                renderable.text_coords[current_poly.text[0]],
                current_poly.normal
            },
            Vertex {
                vp.transform(renderable.transformed_points[current_poly.vert[1]]),
                renderable.text_coords[current_poly.text[1]],
                current_poly.normal
            },
            Vertex {
                vp.transform(renderable.transformed_points[current_poly.vert[2]]),
                renderable.text_coords[current_poly.text[2]],
                current_poly.normal
            }
        };
    }
};
