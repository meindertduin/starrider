#include "core/Application.h"
#include "graphics/Font.h"

int main() {
    ttf_init();
    auto app = Application::get_instance();

    app->initialize({
        .win_width = 1280,
        .win_height = 720,
    });

    for (;;) {

    }
    // app->run();
    ttf_quit();

    return 0;
}

