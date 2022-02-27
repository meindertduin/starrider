#include "../io/BmpReader.h"
#include "Font.h"
#include <iostream>
#include "RenderObject.h"

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
        delete item.second;
    }
}

Glyph BitmapFont::get_glyph(char c) {
    return *m_glyphs[c];
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
    auto buffer = new uint32_t[size];

    // convert form RR format to 5 6 5 with alpha
    for (int i = 0; i < size; i++) {
        // pixels[i] = rgb_from_565(((pixels[i] >> 8) & 0xFF) >> 3, ((pixels[i] >> 16) & 0xFF) >> 2, ((pixels[i] >> 24) & 0xFF) >> 3);
        // pixels[i] = rgb_from_565(0xFF >> 3, 0xFF >> 2, 0xFF >> 3);
        auto val = reinterpret_cast<unsigned char*>(m_face->glyph->bitmap.buffer)[i];
        buffer[i] = (val << 24) | (val << 16) | (val << 8) | val;
        // buffer[i] = 0xFFFFFFFF;
    }

    auto texture = Texture(Format::RED, width, height, buffer);
    delete[] buffer;

    return texture;
}

Glyph TTFFont::get_glyph(char c) const {
    return m_glyphs[c];
}

int TTFFont::get_font_size() const {
    return m_font_size;
}
