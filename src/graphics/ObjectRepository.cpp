#include "ObjectRepository.h"

#include "../io/ObjReader.h"

ObjectRepository::ObjectRepository() {}

ObjectRepository::~ObjectRepository() {
    for (auto object : m_game_objects) {
        delete[] object.local_vertices;
        delete[] object.transformed_vertices;
        delete[] object.texture_coords;
        delete[] object.polygons;

        delete object.texture;
    }
}

RenderObject ObjectRepository::create_game_object(std::string obj_file, std::string texture_file) {
    ObjReader obj_reader;

    auto texture = new Texture();
    texture->load_from_bmp(texture_file);

    if (!obj_reader.read_file(obj_file, texture->width, texture->height)) {
        return -1;
    }

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    object.texture = texture;
    obj_reader.create_render_object(object, object.texture);

    m_game_objects.push_back(object);
    return object;
}
