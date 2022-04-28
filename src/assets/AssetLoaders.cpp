#include "AssetLoaders.h"

#include "../io/BmpReader.h"
#include "../graphics/Texture.h"

#include "Cache.h"

namespace Assets {
    bool load_texture(Cache &cache, const std::string &filename, const AssetOptions &options) {
        BmpReader reader;

        std::vector<Graphics::Texture*> mip_textures;

        auto root_texture = new Graphics::Texture();
        if (!root_texture->load_from_bmp(filename)) {
            throw std::runtime_error("Could not load bmp from file");
        }

        mip_textures.push_back(root_texture);

        auto mip_levels = std::log(root_texture->width) / std::log(2) + 1;

        for (int mip_level = 1; mip_level < mip_levels; mip_level++) {
            auto quarter_texture = mip_textures[mip_level - 1]->quarter_size(1.01f);
            mip_textures.push_back(quarter_texture);
        }

        // cache.set_textures(filename, mip_textures);

        return true;
    }
}
