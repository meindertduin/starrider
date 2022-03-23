#pragma once

#include "RenderObject.h"

#include <vector>
#include <string>

/*
 * Class for managing all the object data. Will also managing
 * allocating and deallocating object data.
 */
class ObjectRepository {
public:
    ObjectRepository();
    ~ObjectRepository();

    ObjectRepository(const ObjectRepository &other) = delete;
    ObjectRepository(ObjectRepository &&other) = delete;

    ObjectRepository& operator=(const ObjectRepository &other) = delete;
    ObjectRepository& operator=(ObjectRepository &&other) = delete;

    RenderObject create_game_object(std::string obj_file, std::string texture_fil);

    int load_texture(std::string path);
    int load_geometry(std::string path);
private:
    std::vector<RenderObject> m_game_objects;

    std::vector<Texture*> m_textures;
    std::vector<Geometry> m_geometries;
};
