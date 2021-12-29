#include "core/Application.h"
#include "graphics/Font.h"

int main() {
    ttf_init();
    auto app = Application::get_instance();

    app->initialize({
        .win_width = 800,
        .win_height = 800,
    });

    app->run();
    ttf_quit();

    return 0;
}

