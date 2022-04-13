#pragma once

#include "RenderObject.h"

#include <vector>
#include <string>

namespace Graphics {

template<class T>
class ObjCollection {
    public:
        ObjCollection(int size) {
            m_size = size;

            std::fill(p_objects, p_objects + size, nullptr);
        }

        T* get_value(int id) const {
            return p_objects[id].get();
        };

        int store_value(std::unique_ptr<T> &&value) {
            for (int i = 0; i < m_size; i++) {
                if (p_objects[i] == nullptr) {
                    p_objects[i] = std::move(value);
                    return i;
                }
            }

            return -1;
        }
    private:
        int m_size;

        std::unique_ptr<T> p_objects[128];
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
    int load_mesh_from_obj(std::string path, MeshAttributes attributes);
private:
    ObjCollection<Texture> m_texture_collection;
    ObjCollection<Mesh> m_mesh_collection;

    int compute_vertex_normals(Graphics::Mesh &object);

    std::vector<RenderObject> m_game_objects;
};
}
