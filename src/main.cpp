#include "core/Application.h"

int main() {
    auto app = Application::get_instance();

    app->initialize({
        .win_width = 800,
        .win_height = 800,
    });

    app->run();

    return 0;
}

