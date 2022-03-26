#pragma once

#include "Core.h"
#include "Renderer.h"
#include "../math/Vector.h"
#include "RenderObject.h"

namespace Graphics {

void init_rasterizer(int levels);

void cleanup_rasterizer();

void rast_set_frame_buffer(int width, int height, Pixel* frame_buffer);

////////// Perfect perspective texture mapping //////////

// Draws triangle with perfect perspective texture mapping with flat shading and using inverse z-buffering.
void draw_perspective_textured_triangle_fsinvzb(RenderListPoly &poly);

// Draws triangle with perfect perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly);

// Draws triangle with perfect perspective texture mapping with intensity gourad shading and using inverse z-buffering with alpha blending.
void draw_perspective_textured_triangle_iinvzb(RenderListPoly &poly, float alpha);

// Draws triangle with perfect perspective texture mapping with flat shading and using inverse z-buffering.
void draw_perspective_textured_triangle_fs(RenderListPoly &poly);

// Draws triangle with perfect perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_perspective_textured_triangle_i(RenderListPoly &poly);




////////// Piecewise perspective texture mapping //////////

// Draws triangle with piecewise perspective texture mapping with flat shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_fsinvzb(RenderListPoly &poly);

// Draws triangle with piecewise perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly);

// Draws triangle with piecewise perspective texture mapping with intensity gourad shading and using inverse z-buffering with alpha blending.
void draw_piecewise_textured_triangle_iinvzb(RenderListPoly &poly, float alpha);

// Draws triangle with piecewise perspective texture mapping with flat shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_fs(RenderListPoly &poly);

// Draws triangle with piecewise perspective texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_piecewise_textured_triangle_i(RenderListPoly &poly);




////////// Affine texture mapping //////////

// Draws triangle with affine texture mapping with flat shading and using inverse z-buffering.
void draw_affine_textured_triangle_fsinvzb(RenderListPoly &poly);

// Draws triangle with affine texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly);

// Draws triangle with affine texture mapping with intensity gourad shading and using inverse z-buffering with alpha blending.
void draw_affine_textured_triangle_iinvzb(RenderListPoly &poly, float alpha);

// Draws triangle with affine texture mapping with flat shading and using inverse z-buffering.
void draw_affine_textured_triangle_fs(RenderListPoly &poly);

// Draws triangle with affine texture mapping with intensity gourad shading and using inverse z-buffering.
void draw_affine_textured_triangle_i(RenderListPoly &poly);




////////// Non-textured rastization methods //////////

// Draws a triangle using solid shading.
void draw_triangle_s(RenderListPoly &poly);

// Draws a triangle using flat shading.
void draw_triangle_fs(RenderListPoly &poly);

// Draws a triangle using intensity gourad shading.
void draw_triangle_i(RenderListPoly &poly);

// Draws a triangle using solid shading and using inverse z-buffering.
void draw_triangle_sinvzb(RenderListPoly &poly);

// Draws a triangle using flat shading and using inverse z-buffering.
void draw_triangle_fsinvzb(RenderListPoly &poly);

// Draws a triangle using intensity gourad shading and using inverse z-buffering.
void draw_triangle_iinvzb(RenderListPoly &poly);

}

