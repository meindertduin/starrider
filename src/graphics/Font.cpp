
#include "../io/BmpReader.h"
#include "Font.h"

BitmapFont::BitmapFont(std::string bitmap_path) {
    Bitmap bitmap(bitmap_path);

    int characters_amount = 126 - '!';
    int font_width = bitmap.width / characters_amount;
    int font_height = bitmap.height;

    Rect src = {
        .width = font_width,
        .height = font_height,
        .x_pos = 0,
        .y_pos = 0,
    };

    for (int i = 0; i < characters_amount; i++) {
        Texture *texture = new Texture();

        src.x_pos = i * font_width;
        Bitmap* glyph_bitmap = bitmap.copy_section(src);
        texture->load_from_bitmap(glyph_bitmap);

        Glyph *glyph = new Glyph();
        glyph->width = font_width;
        glyph->height = font_height;
        glyph->texture = texture;

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
