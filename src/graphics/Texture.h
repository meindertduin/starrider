#pragma once

#include "Core.h"
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

struct RGB {
    uint32_t blue : 8;
    uint32_t green : 8;
    uint32_t red : 8;
};

union Pixel {
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;

    };
    uint32_t value;
};

constexpr void pixel_blend(Pixel &pixel, const Pixel &other) {
    float u_alpha = (float) pixel.alpha / (float) 0xFF;

    pixel.blue = pixel.blue * u_alpha + other.blue * (1.0f - u_alpha);
    pixel.red =  pixel.red * u_alpha + other.red * (1.0f - u_alpha);
    pixel.green = pixel.green * u_alpha + other.green * (1.0f - u_alpha);
    pixel.alpha = pixel.alpha * u_alpha + other.alpha * (1.0f - u_alpha);
}

class Texture {
public:
    Texture() = default;
    Texture(int width, int height, A565Color* data);

    Texture(const Texture &other);
    Texture(Texture &&other) noexcept;

    ~Texture();

public:
    Texture& operator=(const Texture &other);
    Texture& operator=(Texture &&other) noexcept;

    void load_from_bmp(std::string path);
    Texture from_section(Rect src);

    constexpr A565Color get_pixel(int x_pos, int y_pos) const {
        return pixels[width * y_pos + x_pos];
    };

    constexpr A565Color get_pixel_by_shift(int x_pos, int y_pos) const {
        return pixels[(y_pos << m_pitch_shift) + x_pos];
    };

    int width;
    int height;
private:
    int m_pitch_shift = 0;
    uint32_t *pixels = nullptr;
};


