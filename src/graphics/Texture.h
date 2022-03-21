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

    void set_data(int width, int height, A565Color* data);

    Texture* quarter_size(float gamma);

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

    void set_pitch_shift();
};


