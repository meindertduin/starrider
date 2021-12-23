#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library library;

bool init_ttf() {
    FT_Init_FreeType(&library);

    return true;
}
