#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>
#include "Texture.h"
#include "Core.h"

bool init_ttf();
void free_ttf();

struct Glyph {
    Texture texture;
    V2I size;
    V2I bearing;
    long advance;
};

class FontSetTTF {
public:
    FontSetTTF();
    ~FontSetTTF();

    bool load_font(std::string path);
    Glyph get_glyph(char c);
private:
    FT_Face m_face;
    std::map<char, Glyph> m_glyphs;
};
