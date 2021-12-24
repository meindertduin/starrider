#include <stdio.h>

#include "TTF.h"


FT_Library library;

bool init_ttf() {
    auto error = FT_Init_FreeType(&library);
    if (error) {
        return false;
    }


    return true;
}

FontSetTTF::FontSetTTF() {

}

bool FontSetTTF::load_font(std::string path) {
    auto font_error = FT_New_Face(library, "/usr/share/fonts/TTF/Symbola.ttf", 0, &m_face);
    if (font_error == FT_Err_Unknown_File_Format) {
        return false;
    }

    FT_Set_Pixel_Sizes(m_face, 0, 48);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            printf("Error loading char: %c\n", c);
            m_glyphs.clear();

            return false;
        }
    }

    return true;
}

Glyph FontSetTTF::get_glyph(char c) {
    return m_glyphs[c];
}
