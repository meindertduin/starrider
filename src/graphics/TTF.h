#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>

bool init_ttf();

struct Glyph {
};

class FontSetTTF {
public:
    FontSetTTF();

    bool load_font(std::string path);
    Glyph get_glyph(char c);
private:
    FT_Face m_face;
    std::map<char, Glyph> m_glyphs;
};
