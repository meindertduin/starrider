#include "Cache.h"

namespace Assets {
    Cache::Cache() {
        m_loaders.push_back(std::make_pair(Asset::Type::Texture, &load_texture));
    }
}
