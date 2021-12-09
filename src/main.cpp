#include "core/Application.h"

int main() {
    Application app;

    app.initialize({
        .win_width = 800,
        .win_height = 800,
    });

    app.run();

    return 0;
}

