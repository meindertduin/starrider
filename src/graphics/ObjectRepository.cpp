#include "ObjectRepository.h"

#include "../io/ObjReader.h"
#include "../io/MapReader.h"

#include "../assets/Cache.h"

#include <error.h>

namespace Graphics {

ObjectRepository::ObjectRepository() {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects)
        delete[] object.transformed_vertices;
}

RenderObject ObjectRepository::create_render_object(std::string mde_file) {
    RenderObject object;

    auto object_color = A565Color(0xFF, 0, 0, 0);

    Assets::AssetOptions options;
    options.mesh_options.poly_attributes = PolyAttributeTwoSided | PolyAttributeRGB24 |
            PolyAttributeShadeModeIntensityGourad | PolyAttributeShadeModeGouraud | PolyAttributeShadeModeTexture;
    options.mesh_options.poly_state = PolyStateActive;
    options.mesh_options.poly_color = object_color.value;

    m_cache.load_asset(Assets::Asset::Type::Mde, mde_file, options);
    auto mesh = m_cache.get_mesh(mde_file);

    auto objects_count = m_game_objects.size();

    object.textures = load_mip_texture("assets/" + mesh->skins[0]);

    object.mip_levels = object.textures.size();

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.frames_count = mesh->frames_count;

    if (mesh->frames_count > 1) {
        object.attributes |= ObjectAttributeMultiFrame;
    } else {
        object.attributes |= ObjectAttributeSingleFrame;
    }

    object.curr_frame = 0;

    object.vertex_count = mesh->vertex_count;
    object.local_vertices = mesh->vertices;
    object.alpha = 1.0f;

    object.transformed_vertices = new Vertex4D[mesh->vertex_count];

    object.text_count = mesh->text_count;
    object.texture_coords = mesh->text_coords;

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = object_color;
    object.polygons = std::move(mesh->polygons);

    m_game_objects.push_back(object);

    return object;
}

std::vector<Texture*> ObjectRepository::load_mip_texture(std::string path) {
    Assets::AssetOptions options;
    options.texture_options.mipmap = 1;

    m_cache.load_asset(Assets::Asset::Type::Texture, path, options);

    return m_cache.get_textures(path);
}

int ObjectRepository::compute_vertex_normals(Graphics::Mesh &object) {
    int polys_touch_vertices[Graphics::ObjectMaxVertices];
    memset((void*)polys_touch_vertices, 0, sizeof(int) * Graphics::ObjectMaxVertices);

    for (int poly = 0; poly < object.polygons.size(); poly++) {
        if (object.polygons[poly].attributes & Graphics::PolyAttributeShadeModeGouraud) {
            int vi0 = object.polygons[poly].vert[0];
            int vi1 = object.polygons[poly].vert[1];
            int vi2 = object.polygons[poly].vert[2];

            auto line1 = object.vertices[vi0].v
                - object.vertices[vi1].v;

            auto line2 = object.vertices[vi0].v
                - object.vertices[vi2].v;

            auto n = line1.cross(line2);

            object.polygons[poly].n_length = n.length();

            polys_touch_vertices[vi0]++;
            polys_touch_vertices[vi1]++;
            polys_touch_vertices[vi2]++;

            object.vertices[vi0].n += n;
            object.vertices[vi1].n += n;
            object.vertices[vi2].n += n;
        }
    }

    for (int vertex = 0; vertex < object.vertex_count; vertex++) {
        if (polys_touch_vertices[vertex] >= 1) {
            object.vertices[vertex].n /= polys_touch_vertices[vertex];
            object.vertices[vertex].n.normalise();
        }
    }

    return 1;
}

}
