#include "Cache.h"

namespace Assets {
    Cache::Cache() {
        m_loaders.push_back(std::make_pair(Asset::Type::Texture, &load_texture));
        m_loaders.push_back(std::make_pair(Asset::Type::Mde, &load_mde_file));
    }

    void Cache::load_asset(Asset::Type asset_type, const std::string &filename,
            const AssetOptions &options)
    {
        for (auto loader : m_loaders) {
            if (loader.first == asset_type) {
                loader.second(*this, filename, options);
            }
        }
    }

    Graphics::Mesh* Cache::get_mesh(std::string name) {
        auto pair = m_meshes.find(name);

        if (pair == m_meshes.end()) {
            load_asset(Asset::Type::Mde, name, { false });

            return m_meshes.find(name)->second;
        }

        return pair->second;
    }

    void Cache::set_mesh(std::string name, Graphics::Mesh* mesh) {
        m_meshes.insert(std::make_pair(name, mesh));
    }

    void Cache::release_mesh(std::string name) {
        m_meshes.erase(name);
    }

    std::vector<Graphics::Texture*> Cache::get_textures(std::string name) {
        auto pair = m_textures.find(name);

        if (pair == m_textures.end()) {
            load_asset(Asset::Type::Texture, name, { true });

            return m_textures.find(name)->second;
        }

        return pair->second;
    }

    void Cache::set_textures(std::string name, std::vector<Graphics::Texture*> textures) {
        m_textures.insert(std::make_pair(name, textures));
    }

    void Cache::release_textures(std::string name) {
        m_textures.erase(name);
    }
}
