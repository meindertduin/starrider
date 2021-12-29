#pragma once

#include <string>
#include <map>
#include <memory>

#include "Texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

bool ttf_init();
void ttf_quit();

struct Glyph {
    int width;
    int height;
    Texture *texture;
};

class BitmapFont {
public:
    BitmapFont(std::string bitmap_path);
    ~BitmapFont();
    Glyph get_glyph(char c);
private:
    std::map<char, Glyph*> m_glyphs;
};

class TTFFont {
public:
    TTFFont(std::string path);
    ~TTFFont();
    Glyph get_glyph(char c);
    Texture* from_char(char c);
private:
    FT_Face m_face;
    Glyph m_glyphs[128];
};
