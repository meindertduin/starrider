#include "core/Application.h"
#include "graphics/Font.h"

int main() {
    try {
        if (!ttf_init())
            throw std::runtime_error("TTF could not be initialized");
        auto app = Application::get_instance();

        app->initialize({
            .win_width = 1920,
            .win_height = 1080,
        });

        app->run();
        ttf_quit();
    } catch(const std::exception) {
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}

