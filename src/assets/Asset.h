#pragma once

#include <string>

namespace Assets {

    struct AssetOptions {

    };

    struct TextureOptions : public AssetOptions {
        bool mipmap;
    };

    class Asset {
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
