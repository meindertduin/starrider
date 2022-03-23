#include "Texture.h"
#include "Core.h"
#include <exception>

Texture::Texture(const Texture &other) : pixels(nullptr),
    height(other.height), width(other.width) {
    if (other.pixels != nullptr) {
        std::size_t size(other.width * other.height);
        this->pixels = new uint32_t[size];

        std::memcpy(pixels, other.pixels, size);
    }
}

Texture::Texture(Texture &&other) noexcept : pixels(nullptr), width(other.width), height(other.height) {
    pixels = other.pixels;
    other.pixels = nullptr;
}

Texture::Texture(int width, int height, A565Color* data) {
    set_data(width, height, data);
}

Texture::Texture(int width, int height, A565Color*&& data) {
    pixels = reinterpret_cast<uint32_t*>(data);
    this->width = width;
    this->height = height;

    set_pitch_shift();
}

Texture& Texture::operator=(const Texture &other) {
    *this = Texture(other);
    return *this;
}

Texture& Texture::operator=(Texture &&other) noexcept {
    pixels = other.pixels;
    other.pixels = nullptr;
    width = other.width;
    height = other.height;

   return *this;
}

Texture::~Texture() {
    delete[] reinterpret_cast<char*>(pixels);
}

bool Texture::load_from_bmp(std::string path) {
    BmpReader bmp_reader;

    // fucntion call allocates memory for pixels
    if (!bmp_reader.open_file(path)) {
        return false;
    }

    size_t s {0};
    auto bitmap = std::unique_ptr<unsigned char>(nullptr);
    bmp_reader.read_to_buffer(bitmap);

    pixels = reinterpret_cast<uint32_t*>(bitmap.get());

    // Ownership is released because this class manages the pointer for a bit extra performance
    bitmap.release();

    width = bmp_reader.get_width();

    set_pitch_shift();

    height = bmp_reader.get_height();

    return true;
}

Texture Texture::from_section(Rect src) {
    A565Color *data = new A565Color[src.width * src.height];

    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++)
            data[src.width * y + x] = get_pixel(x + src.x_pos, y + src.y_pos);

    return Texture(src.width, src.height, data);
}

void Texture::set_data(int width, int height, A565Color* data) {
    this->width = width;
    this->height = height;

    if (data != nullptr) {
        auto size = width * height;
        this->pixels = new uint32_t[size];

        std::memcpy(pixels, data, size * sizeof(uint32_t));
    }

    set_pitch_shift();
}

Texture* Texture::quarter_size(float gamma) {
    int new_width = width * 0.5;
    int new_height = height * 0.5f;

    A565Color *data = new A565Color[new_width * new_height];

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            uint32_t r0, g0, b0,
                  r1, g1, b1,
                  r2, g2, b2,
                  r3, g3, b3;

            int r_avg, g_avg, b_avg;

            get_pixel_by_shift(x * 2 + 0, y * 2 + 0).rgb565_from_16bit(r0, g0, b0);
            get_pixel_by_shift(x * 2 + 1, y * 2 + 0).rgb565_from_16bit(r1, g1, b1);
            get_pixel_by_shift(x * 2 + 0, y * 2 + 1).rgb565_from_16bit(r2, g2, b2);
            get_pixel_by_shift(x * 2 + 1, y * 2 + 1).rgb565_from_16bit(r3, g3, b3);

            r_avg = (int)(0.5f + gamma * (float)(r0 + r1 + r2 + r3) / 4.0f);
            g_avg = (int)(0.5f + gamma * (float)(g0 + g1 + g2 + g3) / 4.0f);
            b_avg = (int)(0.5f + gamma * (float)(b0 + b1 + b2 + b3) / 4.0f);

            if (r_avg > 31) r_avg = 31;
            if (g_avg > 63) g_avg = 63;
            if (b_avg > 31) b_avg = 31;

            data[x + y * new_width] = A565Color(r_avg << 3, g_avg << 2, b_avg << 3);
        }
    }

    auto result = new Texture(new_width, new_height, data);
    delete[] data;

    return result;
}

void Texture::set_pitch_shift() {
    switch(width) {
        case 4:
           m_pitch_shift = 2;
           break;
        case 8:
           m_pitch_shift = 3;
           break;
        case 16:
           m_pitch_shift = 4;
           break;
        case 32:
           m_pitch_shift = 5;
           break;
        case 64:
           m_pitch_shift = 6;
           break;
        case 128:
           m_pitch_shift = 7;
           break;
        case 256:
           m_pitch_shift = 8;
           break;
        default:
           std::runtime_error("Only widths in orders of 2^n until 256 are supported.");
    }
}
