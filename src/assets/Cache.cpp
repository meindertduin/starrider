#include "Cache.h"

namespace Assets {
    Cache::Cache() {
        m_loaders.push_back(std::make_pair(Asset::Type::Texture, &load_texture));
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
