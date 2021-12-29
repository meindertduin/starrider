#include "../io/BmpReader.h"
#include "Font.h"
#include <iostream>

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

BitmapFont::BitmapFont(std::string bitmap_path) {
    Texture font_texture;

    font_texture.load_from_bmp(bitmap_path);
    int characters_amount = 126 - '!';
    int font_width = font_texture.width / characters_amount;
    int font_height = font_texture.height;

    Rect src = {
        .width = font_width,
        .height = font_height,
        .x_pos = 0,
        .y_pos = 0,
    };

    for (int i = 0; i < characters_amount; i++) {
        src.x_pos = i * font_width;

        Glyph *glyph = new Glyph();
        glyph->width = font_width;
        glyph->height = font_height;
        glyph->texture = font_texture.from_section(src);

        m_glyphs.insert(std::pair<char, Glyph*>(i + '!', glyph));
    }
}

BitmapFont::~BitmapFont() {
    for(auto item : m_glyphs) {
        delete item.second->texture;
        delete item.second;
    }
}

Glyph BitmapFont::get_glyph(char c) {
    return *m_glyphs[c];
}

TTFFont::TTFFont(std::string path) {
    FT_New_Face(library, path.c_str(), 0, &m_face);
    FT_Set_Pixel_Sizes(m_face, 0, 32);

    for (char c = 0; c < 127; c++) {
        Glyph glyph;

        glyph.texture = from_char(c);
        glyph.width = m_face->glyph->bitmap.width;
        glyph.height = m_face->glyph->bitmap.rows;

        m_glyphs[c] = glyph;
    }
}

Texture* TTFFont::from_char(char c) {
    FT_Set_Pixel_Sizes(m_face, 0, 32);

    FT_Load_Char(m_face, c, FT_LOAD_RENDER);
    Texture *texture = new Texture();
    texture->load_from_bitmap(Format::RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, m_face->glyph->bitmap.buffer);
    return texture;
}

TTFFont::~TTFFont() {
    FT_Done_Face(m_face);
}

Glyph TTFFont::get_glyph(char c) {
    // TODO: as i understand the buffer for the face->glyph needs to be re-rendered everytime it's used.
    // This may or may not be optimizable
    FT_Load_Char(m_face, c, FT_LOAD_RENDER);
    return m_glyphs[c];
}
