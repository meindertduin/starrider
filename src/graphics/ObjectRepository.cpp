#include "ObjectRepository.h"

#include "../io/ObjReader.h"

ObjectRepository::ObjectRepository() : m_texture_collection(128), m_mesh_collection(128) {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects) {
        delete[] object.transformed_vertices;
    }
}

RenderObject ObjectRepository::create_game_object(std::string obj_file, std::string texture_file) {
    ObjReader obj_reader;

    auto texture_id = load_texture(texture_file);
    auto texture = m_texture_collection.get_value(texture_id);

    auto mesh_id = load_geometry(obj_file);
    auto mesh = m_mesh_collection.get_value(mesh_id);

    if (!obj_reader.read_file(obj_file)) {
        return -1;
    }

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    object.textures.push_back(texture);
    object.mip_levels = std::log(texture->width) / std::log(2) + 1;

    for (int mip_level = 1; mip_level < object.mip_levels; mip_level++) {
        auto quarter_texture = object.textures[mip_level - 1]->quarter_size(1.01f);
        object.textures.push_back(quarter_texture);
        auto id = m_texture_collection.store_value(std::unique_ptr<Texture>(quarter_texture));
        quarter_texture->id = id;
    }

    // TODO abstract the object creation
    object.state = ObjectStateActive | ObjectStateVisible;
    object.attributes |= ObjectAttributeSingleFrame;
    object.curr_frame = 0;
    object.frames_count = 1;

    object.vertex_count = mesh->vertex_count;
    object.local_vertices = mesh->vertices;
    object.alpha = 1.0f;

    object.transformed_vertices = new Vertex4D[mesh->vertex_count];

    object.text_count = mesh->text_count;
    object.texture_coords = mesh->text_coords;

    object.head_local_vertices = &object.local_vertices[0];
    object.head_transformed_vertices = &object.transformed_vertices[0];

    object.color = A565Color(0xFF, 0, 0, 0);

    object.polygons = std::move(mesh->polygons);

    m_game_objects.push_back(object);
    return object;
}

int ObjectRepository::load_texture(std::string path) {
    auto texture = new Texture();
    if (!texture->load_from_bmp(path)) {
        return -1;
    }

    auto id = m_texture_collection.store_value(std::unique_ptr<Texture>(texture));
    texture->id = id;
    return id;
}

int ObjectRepository::load_geometry(std::string path) {
    ObjReader obj_reader;

    if (!obj_reader.read_file(path)) {
        return -1;
    }

    auto mesh = new Mesh {};
    obj_reader.extract_content(*mesh);

    auto id = m_mesh_collection.store_value(std::unique_ptr<MeshType>(mesh));
    mesh->id = id;

    return id;
}
