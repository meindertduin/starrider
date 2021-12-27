#pragma once

#include <string>
#include <map>

#include "Texture.h"

struct Glyph {
    int width;
    int height;
    Texture texture;
};

class BitmapFont {
public:
    BitmapFont(std::string bitmap_path);
private:
    std::map<char, Glyph> m_glyphs;
};
