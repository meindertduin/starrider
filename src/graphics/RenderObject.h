#pragma once

#include "Core.h"
#include "../math/Vector.h"
#include "Texture.h"

namespace Graphics {

using Math::Point4D;
using Math::Point2D;
using Math::V4D;

const uint16_t ObjectStateNull = 0;
const uint16_t ObjectStateActive = 1;
const uint16_t ObjectStateVisible = 1 << 1;
const uint16_t ObjectStateCulled = 1 << 2;
const uint16_t ObjectStateLit = 1 << 3;

const uint16_t ObjectAttributeSingleFrame = 1;
const uint16_t ObjectAttributeMultiFrame = 1 << 1;
const uint16_t ObjectAttributeTextures = 1 << 2;

const uint16_t PolyStateNull = 0;
const uint16_t PolyStateActive = 1;
const uint16_t PolyStateClipped = 1 << 1;
const uint16_t PolyStateBackface = 1 << 2;
const uint16_t PolyStateLit = 1 << 3;

const uint16_t PolyAttributeTwoSided = 1;
const uint16_t PolyAttributeTransparent = 1 << 1;
const uint16_t PolyAttributeEightBitColor = 1 << 2;
const uint16_t PolyAttributeRGB16 = 1 << 3;
const uint16_t PolyAttributeRGB24 = 1 << 4;
const uint16_t PolyAttributeShadeModeFlat = 1 << 5;
const uint16_t PolyAttributeShadeModeGouraud = 1 << 6;
const uint16_t PolyAttributeShadeModeIntensityGourad = 1 << 7;
const uint16_t PolyAttributeShadeModeConstant = 1 << 8;
const uint16_t PolyAttributeShadeModeTexture = 1 << 9;
const uint16_t PolyAttributeEnableMaterial = 1 << 10;
const uint16_t PolyAttributeDisableMaterial = 1 << 11;

const uint16_t MaterialStateTransparent = 1 << 1;
const uint16_t MaterialStateEightBitColor = 1 << 2;
const uint16_t MaterialStateRGB16 = 1 << 3;
const uint16_t MaterialStateRGB24 = 1 << 4;
const uint16_t MaterialStateShadeModeConstant = 1 << 5;
const uint16_t MaterialStateShadeModeEmmisive = 1 << 6;
const uint16_t MaterialStateShadeModeFlat = 1 << 7;
const uint16_t MaterialStateShadeModeGouraud = 1 << 8;
const uint16_t MaterialStateShateModeSimplePhong = 1 << 9;
const uint16_t MaterialStateShadeModeTexture = 1 << 10;

const uint16_t MaterialAttributeActive = 1;

const uint16_t LightAttributeAmbient = 1;
const uint16_t LightAttributeInfinite = 1 << 1;
const uint16_t LightAttributePoint = 1 << 2;
const uint16_t LightAttributeSpoitLight1 = 1 << 3;
const uint16_t LightAttributeSpoitLight2 = 1 << 4;

const uint16_t ShadeModePure = 0x0000;
const uint16_t ShadeModeConstant = 0x0000;
const uint16_t ShadeModeFlat = 0x2000;
const uint16_t ShadeModeGouraud = 0x4000;
const uint16_t ShadeModePhong = 0x6000;
const uint16_t ShadeModeFastPhong = 0x6000;

const u_int16_t LightStateOn = 1;
const u_int16_t LightStateOff = 0;

const uint16_t VertexAttributeNull = 0;
const uint16_t VertexAttributePoint = 1 << 1;
const uint16_t VertexAttributeNormal = 1 << 2;
const uint16_t VertexAttributeTexture = 1 << 3;

const int ObjectMaxVertices = 32768;
const int ObjectMaxPolys = 8192;
const int RenderListMaxPolys = 32768;

static constexpr int MaxMaterials = 256;

typedef struct RGBA_Type {
    union {
        uint32_t rgba;
        uint8_t rgba_m[4];
        struct {
            u_int8_t a, b, g, r;
        };
    };

    RGBA_Type() = default;
    RGBA_Type(uint32_t rgba) : rgba(rgba) {  }
    RGBA_Type(uint8_t r, uint8_t g, u_int8_t b, u_int8_t a) : r(r), g(g), b(b), a(a) {}

    constexpr uint32_t to_argb_bit() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    constexpr uint32_t to_argb_bit(float i) {
        return ((uint32_t)(a * i) << 24) | ((uint32_t)(r * i) << 16) | ((uint32_t)(g * i) << 16) | ((uint32_t) (b * i));
    }
} RGBA;

typedef struct Vertext4D_Type {
    Point4D v;
    V4D n;
    Point2D t;
    float i;
    uint16_t attributes;
} Vertex4D;

typedef struct Polygon_Type {
    uint16_t state;
    uint16_t attributes;
    A565Color color;
    A565Color lit_color[3];

    int mati;
    Texture *texture;

    Vertex4D *vertices;
    Point2D *text_coords;

    int vert[3];
    int text[3];
    float n_length;
    // TODO can be deleted when using the RenderListPoly at this stage
    V4D normal;
} Polygon;

typedef struct RenderListPoly_Type {
    uint16_t state;
    uint16_t attributes;
    A565Color color;
    A565Color lit_color[3];
    Texture *texture;
    int mati;

    float n_length;
    V4D normal;
    float alpha;

    float avg_z;
    Vertex4D verts[3];
    Vertex4D trans_verts[3];
} RenderListPoly;

struct StaticRenderObject {
    int state;
    int attributes;

    int vertex_count;
    int text_count;
    int mip_levels;

    std::vector<Texture*> textures;

    float alpha;
    A565Color color;

    Transform transform;

    Vertex4D *local_vertices;
    Point2D *texture_coords;

    std::vector<Polygon> polygons;
};

typedef struct RenderObject_Type : public StaticRenderObject {
    uint16_t state;
    uint16_t attributes;

    int mati;

    int frames_count;
    int curr_frame;

    Vertex4D *transformed_vertices;

    Vertex4D *head_local_vertices;
    Vertex4D *head_transformed_vertices;

    int set_frame(int frame);

    // This function may reset the frames
    void next_frame();
} RenderObject;


struct TerrainTile {
    int x_pos;
    int y_pos;

    std::vector<int> polygon_indices;
};

struct TerrainObject {
    int grid_width;
    int grid_height;

    StaticRenderObject object;

    std::vector<TerrainTile> tiles;
};

typedef struct MeshType {
    int id;
    int vertex_count;
    Vertex4D *vertices;
    int frames_count;

    int text_count;
    Point2D *text_coords;

    std::vector<Polygon> polygons;
    std::vector<std::string> skins;

    MeshType() = default;
    MeshType(const MeshType &other) {
        vertex_count = other.vertex_count;
        text_count = other.text_count;

        std::memcpy(vertices, other.vertices, vertex_count);
        std::memcpy(text_coords, other.text_coords, text_count);

        polygons = other.polygons;
    }

    MeshType(MeshType &&other) {
        vertex_count = other.vertex_count;
        text_count = other.text_count;

        vertices = other.vertices;
        text_coords = other.text_coords;

        polygons = std::move(other.polygons);
    }

    MeshType& operator=(const MeshType &other) {
        *this = MeshType(other);
        return *this;
    }

    MeshType& operator=(MeshType &&other) {
        *this = MeshType(other);
        return *this;
    }

    ~MeshType() {
        delete[] vertices;
        delete[] text_coords;
    }
} Mesh;

struct MeshAttributes {
    uint16_t poly_state;
    uint16_t poly_attributes;
    A565Color poly_color;
};

typedef struct Material_Type {
    int id;
    uint16_t state;
    uint16_t attributes;
    A565Color color;
    RGBA ra, rd, rs; // reflectivities
    Texture *texutre = nullptr;
} Material;

extern Material_Type g_materials[MaxMaterials];

void reset_materials();

constexpr bool render_polygon_avg_sort(const RenderListPoly &a, const RenderListPoly &b) {
    return (a.trans_verts[0].v.z * a.trans_verts[1].v.z * a.trans_verts[2].v.z) >
        (b.trans_verts[0].v.z * b.trans_verts[1].v.z * b.trans_verts[2].v.z);
}

union Pixel {
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;

    };
    uint32_t value;
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
    std::vector<RenderListPoly> render_list;
    float perfect_dist;
    float piecewise_dist;

    float *inv_z_buffer;
    Pixel *frame_buffer;

    int min_clip_x;
    int max_clip_x;

    int min_clip_y;
    int max_clip_y;
};
}

