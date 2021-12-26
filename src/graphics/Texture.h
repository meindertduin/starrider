#pragma once

#include <string>

struct Bitmap;

class Texture {
public:
    Texture();
    ~Texture();
    void test_load();
    void load_from_bmp(std::string path);

    uint32_t get_pixel(int x_pos, int y_pos, float light_amount) const;
    uint32_t get_pixel(int x_pos, int y_pos) const;
    int width;
    int height;
private:
    Bitmap *p_bitmap = nullptr;
};


