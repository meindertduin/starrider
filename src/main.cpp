#include "core/Application.h"
#include "graphics/Font.h"
#include "graphics/Rasterizer.h"

#include "math/Core.h"
#include "io/Logger.h"

int main() {
    try {
        Math::build_lookup_tables();
        Graphics::build_rgb_lookup(18);

        Logger::initialize("log.txt");

        if (!ttf_init())
            throw std::runtime_error("TTF could not be initialized");
        auto app = Application::get_instance();

        app->initialize({
            .win_width = 1920,
            .win_height = 1080,
        });

        app->run();
        ttf_quit();
        Graphics::cleanup_rgb_lookup();
    } catch(const std::exception) {
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}

