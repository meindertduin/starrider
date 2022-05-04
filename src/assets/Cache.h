#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Asset.h"
#include "../graphics/RenderObject.h"

#include "AssetLoaders.h"

namespace Assets {
    class Cache final {
        public:
            Cache();
            ~Cache();

            Cache(const Cache &cache) = delete;
            Cache(Cache &&cache) = delete;

            Cache& operator=(const Cache &cache) = delete;
            Cache& operator=(Cache &&cache) = delete;

            void load_asset(Asset::Type asset_type, const std::string &filename,
                    const AssetOptions &options);

            Graphics::Mesh* get_mesh(std::string name) const;
            void set_mesh(std::string name, std::unique_ptr<Graphics::Mesh> mesh);
            void release_mesh(std::string name);

            std::vector<Graphics::Texture*> get_textures(std::string name) const;
            void set_textures(std::string name, Graphics::MipTexturesList &&textures);
            void release_textures(std::string name);

        private:
            std::unordered_map<std::string, Graphics::MipTexturesList> m_textures;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Mesh>> m_meshes;

            std::vector<std::pair<Asset::Type, AssetLoader>> m_loaders;
    };
}
