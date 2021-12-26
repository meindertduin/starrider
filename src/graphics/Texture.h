#pragma once

#include <string>

struct Bitmap;

union Pixel {
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;
    } rgba;
    uint32_t value;
};

class Texture {
public:
    Texture();
    ~Texture();
    void test_load();
    void load_from_bmp(std::string path);

    Pixel get_pixel(int x_pos, int y_pos, float light_amount) const;
    Pixel get_pixel(int x_pos, int y_pos) const;
    int width;
    int height;
private:
    Bitmap *p_bitmap = nullptr;
};


