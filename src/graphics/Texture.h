#pragma once

#include <string>
#include "../io/BmpReader.h"

#include <cstring>

enum class Format;

struct Rect {
    int width;
    int height;
    int x_pos;
    int y_pos;
};

enum class Format {
    RED,
    RGB,
    RGBA,
};

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

struct RGB {
    uint32_t blue : 8;
    uint32_t green : 8;
    uint32_t red : 8;
};


union Pixel {
    RGBA rgba;
    uint32_t value;
};

class Texture {
public:
    Texture();
    Texture(Format format, int width, int height, void* data);

    Texture(const Texture &other);
    Texture(Texture &&other) noexcept;

    ~Texture();

public:
    Texture& operator=(const Texture &other);
    Texture& operator=(Texture &&other) noexcept;

    void load_from_bmp(std::string path);
    Texture from_section(Rect src);

    Pixel get_pixel(int x_pos, int y_pos, float light_amount) const;
    Pixel get_pixel(int x_pos, int y_pos) const;

    int width;
    int height;
private:
    void *pixels = nullptr;
    Format format;

    inline uint32_t get_pixel_value(int x_pos, int y_pos) const;
};


