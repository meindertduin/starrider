#include "../io/BmpReader.h"
#include "Font.h"
#include <iostream>

namespace Graphics {

FT_Library library;

bool ttf_init() {
    if (FT_Init_FreeType(&library)) {
        std::cerr << "add_ttf() Error: could not initialize TTF with freetype 2." << std::endl;
        return false;
    }

    return true;
}

void ttf_quit() {
    FT_Done_FreeType(library);
}

TTFFont::TTFFont(std::string path, int size) : m_font_size(size) {
    FT_New_Face(library, path.c_str(), 0, &m_face);
    FT_Set_Pixel_Sizes(m_face, size, size);

    for (char c = 0; c < 127; c++) {
        Glyph glyph;

        glyph.texture = from_char(c);
        glyph.width = m_face->glyph->bitmap.width;
        glyph.height = m_face->glyph->bitmap.rows;
        glyph.bearing = V2I(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
        glyph.advance = m_face->glyph->advance.x / 64; // advance is measured in 1/64 of a pixel

        m_glyphs[c] = glyph;
    }
}


TTFFont::~TTFFont() {
    FT_Done_Face(m_face);
}

Texture TTFFont::from_char(char c) {
    FT_Set_Pixel_Sizes(m_face, 0, 32);
    FT_Load_Char(m_face, c, FT_LOAD_RENDER);

    auto width = m_face->glyph->bitmap.width;
    auto height = m_face->glyph->bitmap.rows * 4;

    int size = width * height;
    auto buffer = new A565Color[size];

    // convert form RR format to 5 6 5 with alpha
    for (int i = 0; i < size; i++) {
        auto val = reinterpret_cast<unsigned char*>(m_face->glyph->bitmap.buffer)[i];
        buffer[i] = A565Color(val, val, val, val);
    }

    auto texture = Texture(width, height, buffer);
    delete[] buffer;

    return texture;
}

Glyph TTFFont::get_glyph(char c) const {
    return m_glyphs[c];
}

int TTFFont::get_font_size() const {
    return m_font_size;
}
}
