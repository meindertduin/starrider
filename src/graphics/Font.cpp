#include "../io/BmpReader.h"
#include "Font.h"

BitmapFont::BitmapFont(std::string bitmap_path) {
    Bitmap bitmap(bitmap_path);

    int font_width = 126 - '!';
    int font_height = bitmap.height;

    for (int c = '!'; c < 127; c++) {
        Texture texture;
    }
}
