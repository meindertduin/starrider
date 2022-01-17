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

    constexpr Pixel get_pixel(int x_pos, int y_pos, float light_amount) const {
        Pixel r = {
            .value = get_pixel_value(x_pos, y_pos),
        };

        r.rgba.blue *= light_amount;
        r.rgba.red *= light_amount;
        r.rgba.green *= light_amount;

        return r;
    }
    constexpr Pixel get_pixel(int x_pos, int y_pos) const {
        return {
            .value = get_pixel_value(x_pos, y_pos),
        };
    };

    int width;
    int height;
private:
    void *pixels = nullptr;
    Format format;

    constexpr uint32_t get_pixel_value(int x_pos, int y_pos) const {
        switch(format) {
            case Format::RED:
                {
                    uint32_t val = reinterpret_cast<unsigned char*>(pixels)[width * y_pos + x_pos];
                    return (val << 24) | (val << 16) | (val << 8) | val;
                }
            case Format::RGBA:
                return static_cast<uint32_t*>(pixels)[width * y_pos + x_pos];
            case Format::RGB:
                {
                    auto rgb =  static_cast<RGB*>(pixels)[width * y_pos + x_pos];
                    return (0x000000FF << 24) | (rgb.red << 16) | (rgb.green << 8) | (rgb.blue);
                }
            default:
                return 0;
        }
    }
};


