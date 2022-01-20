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

    ObjectRepository(const ObjectRepository &other) = delete;
    ObjectRepository(ObjectRepository &&other) = delete;

    ObjectRepository& operator=(const ObjectRepository &other) = delete;
    ObjectRepository& operator=(ObjectRepository &&other) = delete;

    /* Returns -1 if no object could be created */
    int create_game_object(std::string obj_file, std::string texture_fil);
private:
    std::vector<RenderObject> m_game_objects;
};
