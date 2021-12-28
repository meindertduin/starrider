#pragma once

#include <string>
#include "../io/BmpReader.h"

enum class Format;

struct Rect {
    int width;
    int height;
    int x_pos;
    int y_pos;
};

enum class Format {
    RED,
    RG,
    RGB,
    RGBA,
};

struct Bitmap {
    void *pixels = nullptr;
    int width;
    int height;
    Format format;

    Bitmap() {}
    Bitmap(Format format, int width, int height, void* data) {
        this->format = format;
        this->width = width;
        this->height = height;
        pixels = data;
    }

    Bitmap(std::string path) {
        BmpReader bmp_reader;

        // fucntion call allocates memory for pixels
        bmp_reader.read_file(path, pixels);
        format = Format::RGBA;

        width = bmp_reader.get_width();
        height = bmp_reader.get_height();
    }

    uint32_t get_value(int x_pos, int y_pos) const {
        switch(format) {
            case Format::RED:
                {
                    uint32_t val = static_cast<uint8_t*>(pixels)[width * y_pos + x_pos];
                    return (0x0000FF00 + val) << 16;
                }
            case Format::RGBA:
                return static_cast<uint32_t*>(pixels)[width * y_pos + x_pos];
            default:
                return 0;
        }
    }

    Bitmap* copy_section(const Rect &src) {
        if (src.x_pos + src.width <= width && src.y_pos + src.height <= height) {
            void *data;
            switch(format) {
                case Format::RGBA:
                    data = new uint32_t[src.width * src.height];
                    for (int y = 0; y < src.height; y++)
                        for (int x = 0; x < src.width; x++)
                            static_cast<uint32_t*>(data)[src.width * y + x] = static_cast<uint32_t*>(pixels)[src.width + (src.y_pos + y) + (src.x_pos + x)];
                    break;
                default:
                    break;
            }

            return new Bitmap(format, src.width, src.height, data);
        } else {
            return new Bitmap();
        }
    }
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
    void load_from_bitmap(Bitmap *bitmap);

    Pixel get_pixel(int x_pos, int y_pos, float light_amount) const;
    Pixel get_pixel(int x_pos, int y_pos) const;
    int width;
    int height;
private:
    Bitmap m_bitmap;
};


