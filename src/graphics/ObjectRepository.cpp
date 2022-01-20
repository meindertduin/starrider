#include "ObjectRepository.h"

#include "../io/ObjReader.h"

ObjectRepository::ObjectRepository() {}

int ObjectRepository::create_game_object(std::string obj_file, std::string texture_fil) {
    ObjReader obj_reader;

    if (obj_reader.read_file(obj_file)) {
        return -1;
    }

    auto objects_count = m_game_objects.size();
    RenderObject object { static_cast<int>(objects_count > 0 ? objects_count - 1 : 0) };

    obj_reader.create_render_object(object);


}
