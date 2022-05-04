#include "Cache.h"

namespace Assets {
    Cache::Cache() {
        m_loaders.push_back(std::make_pair(Asset::Type::Texture, &load_texture));
        m_loaders.push_back(std::make_pair(Asset::Type::Mde, &load_mde_file));
    }

    Cache::~Cache() {
        for (auto mip_textures : m_textures)
            for (auto texture : mip_textures.second)
                delete texture;
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

    Graphics::Mesh* Cache::get_mesh(std::string name) const {
        auto pair = m_meshes.find(name);

        return pair->second.get();
    }

    void Cache::set_mesh(std::string name, std::unique_ptr<Graphics::Mesh> mesh) {
        m_meshes.insert(std::make_pair(name, std::move(mesh)));
    }

    void Cache::release_mesh(std::string name) {
        m_meshes.erase(name);
    }

    std::vector<Graphics::Texture*> Cache::get_textures(std::string name) const {
        auto pair = m_textures.find(name);

        return pair->second;
    }

    void Cache::set_textures(std::string name, std::vector<Graphics::Texture*> textures) {
        m_textures.insert(std::make_pair(name, textures));
    }

    void Cache::release_textures(std::string name) {
        m_textures.erase(name);
    }
}
