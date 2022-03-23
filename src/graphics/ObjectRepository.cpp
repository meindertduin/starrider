#include "ObjectRepository.h"

#include "../io/ObjReader.h"

ObjectRepository::ObjectRepository() {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects) {
        delete[] object.local_vertices;
        delete[] object.transformed_vertices;
        delete[] object.texture_coords;
    }

    for (auto texture : m_textures) {
        delete texture;
    }
}

RenderObject ObjectRepository::create_game_object(std::string obj_file, std::string texture_file) {
    ObjReader obj_reader;

    auto geometry_id = load_geometry(obj_file);
    auto texture_id = load_texture(texture_file);

    if (geometry_id == -1 ||texture_id == -1) {
        return -1;
    }

    auto obj_content = m_geometries[geometry_id];
    auto texture = m_textures[texture_id];

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    object.textures.push_back(texture);
    object.mip_levels = std::log(texture->width) / std::log(2) + 1;
    object.mip_levels = 1;

    for (int mip_level = 1; mip_level < object.mip_levels; mip_level++) {
        auto quarter_texture = object.textures[mip_level - 1]->quarter_size(1.01f);

        object.textures.push_back(quarter_texture);
        m_textures.push_back(quarter_texture);
    }

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.attributes |= ObjectAttributeSingleFrame;
    object.curr_frame = 0;
    object.frames_count = 1;

    object.vertex_count = obj_content.vertex_count;
    object.local_vertices = obj_content.vertices;

    object.transformed_vertices = new Vertex4D[obj_content.vertex_count];

    object.text_count = obj_content.text_count;
    object.texture_coords = obj_content.text_coords;

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = A565Color(0xFF, 0, 0, 0);

    object.polygons = std::move(obj_content.polygons);

    m_game_objects.push_back(object);
    return object;
}

int ObjectRepository::load_texture(std::string path) {
    auto texture = new Texture();
    if (!texture->load_from_bmp(path)) {
        return -1;
    }

    m_textures.push_back(texture);

    // TODO create a better id system and texture collection for that matter
    return m_textures.size() -1;
}

int ObjectRepository::load_geometry(std::string path) {
    ObjReader obj_reader;

    if (!obj_reader.read_file(path)) {
        return -1;
    }

    auto geometry = obj_reader.extract_content();
    m_geometries.push_back(geometry);

    return m_geometries.size() - 1;
}
