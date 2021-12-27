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

void free_ttf() {
    FT_Done_FreeType(library);
}

FontSetTTF::FontSetTTF() {
}

FontSetTTF::~FontSetTTF() {
    FT_Done_Face(m_face);

    for (auto glyph : m_glyphs) {
        delete glyph.second;
    }
}

bool FontSetTTF::load_font(std::string path) {
    auto font_error = FT_New_Face(library, path.c_str(), 0, &m_face);
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

        Texture texture;
        texture.load_from_bitmap(Format::RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows,
                m_face->glyph->bitmap.buffer);

        texture.width = m_face->glyph->bitmap.width;
        texture.height = m_face->glyph->bitmap.rows;

        Glyph *glyph = new Glyph();
        *glyph = {
            .texture = texture,
            .size = V2I(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
            .bearing = V2I(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
            .advance = m_face->glyph->advance.x,
        };

        m_glyphs.insert(std::pair<char, Glyph*>(c, glyph));
    }

    return true;
}

Glyph FontSetTTF::get_glyph(char c) {
    return *m_glyphs[c];
}
