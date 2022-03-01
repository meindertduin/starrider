#pragma once

#include <string>
#include <map>
#include <memory>

#include "Texture.h"
#include "Core.h"

#include <ft2build.h>
#include FT_FREETYPE_H

bool ttf_init();
void ttf_quit();

struct Glyph {
    int width;
    int height;
    V2I bearing;
    Texture texture;
    int advance; // 1 = 1/64 of a pixel. See freetype documentation
};

class TTFFont {
public:
    TTFFont(std::string path, int size);
    ~TTFFont();
    Glyph get_glyph(char c) const;
    int get_font_size() const;
private:
    FT_Face m_face;
    Glyph m_glyphs[127];
    int m_font_size;

    Texture from_char(char c);
};
