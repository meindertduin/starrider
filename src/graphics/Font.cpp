
#include "../io/BmpReader.h"
#include "Font.h"

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
