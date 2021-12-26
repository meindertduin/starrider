#include "core/Application.h"
#include "graphics/TTF.h"

int main() {
    init_ttf();
    auto app = Application::get_instance();

    app->initialize({
        .win_width = 800,
        .win_height = 800,
    });

    app->run();
    free_ttf();

    return 0;
}

