#pragma once

#include <functional>

#include "Asset.h"

namespace Assets {
    class Cache;

    using AssetLoader = std::function<bool(Cache&, const std::string&, const AssetOptions&)>;

    bool load_texture(Cache &cache, const std::string &filename, const AssetOptions &options);

    bool load_mde_file(Cache &cache, const std::string &filename, const AssetOptions &options);
}
