#pragma once

#include <string>

struct Bitmap;
enum class Format;

struct RGBA {
    uint32_t blue : 8;
    uint32_t green : 8;
    uint32_t red : 8;
    uint32_t alpha : 8;

    void blend(const RGBA &r) {
        float u_alpha = (float) alpha / (float) 0xFF;

        blue = blue * u_alpha + r.blue * (1.0f - u_alpha);
        red =  red * u_alpha + r.red * (1.0f - u_alpha);
        green = green * u_alpha + r.green * (1.0f - u_alpha);
        alpha = alpha * u_alpha + r.alpha * (1.0f - u_alpha);
    }
};

union Pixel {
    RGBA rgba;
    uint32_t value;
};

class Texture {
public:

    Texture();
    ~Texture();
    void test_load();
    void load_from_bmp(std::string path);
    void load_from_bitmap(Format format, int width, int height, void* data);

    Pixel get_pixel(int x_pos, int y_pos, float light_amount) const;
    Pixel get_pixel(int x_pos, int y_pos) const;
    int width;
    int height;
private:
    Bitmap *p_bitmap = nullptr;
};


