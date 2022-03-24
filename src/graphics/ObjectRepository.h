#pragma once

#include "RenderObject.h"

#include <vector>
#include <string>


template<class T>
class ObjCollection {
    public:
        ObjCollection(int size) {
            m_size = size;

            std::fill(p_objects, p_objects + size, nullptr);
        }

        ~ObjCollection() {
            for (int i = 0; i < m_size; i++) {
                delete p_objects[i];
            }
        }

        T* get_value(int id) const {
            return p_objects[id];
        };

        int store_value(T* value) {
            for (int i = 0; i < m_size; i++) {
                if (p_objects[i] == nullptr) {
                    p_objects[i] = value;
                    return i;
                }
            }

            return -1;
        }
    private:
        int m_size;

        T* p_objects[128];
};

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
    ObjCollection<Texture> m_texture_collection;
    std::vector<RenderObject> m_game_objects;

    std::vector<Geometry> m_geometries;
};
