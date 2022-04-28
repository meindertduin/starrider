#pragma once

#include <string>

namespace Assets {

    struct AssetOptions {
        bool mipmap;
    };

    class Asset final {
        public:
            enum class Type {
                Texture,
                Mde,
            };

            Asset(Type type, const std::string &name, const std::string filename,
                    const AssetOptions options) :
                type(type), name (name), filename(filename), options(options) {  }


            Type type;
            AssetOptions options;
            std::string name;
            std::string filename;
    };
}
