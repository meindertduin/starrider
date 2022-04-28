#pragma once

#include <string>

namespace Assets {

    struct AssetOptions {
        union {
            struct {
                int mipmap;
                int padding;
            } texture_options;

            struct {
                uint32_t poly_state : 16;
                uint32_t poly_attributes : 16;

                uint32_t poly_color;
            } mesh_options;

            uint32_t values[2];
        };
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
