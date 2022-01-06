#include "Texture.h"
#include "Core.h"


Texture::Texture() {
}

Texture::Texture(const Texture &other) : pixels(nullptr),
    height(other.height), width(other.width), format(other.format)
{
    if (other.pixels != nullptr) {
        std::size_t size(other.width * other.height);
        switch (other.format) {
            case Format::RED:
                this->pixels = new unsigned char[size];
                break;
            case Format::RGBA:
                this->pixels = new uint32_t[size];
                break;
        }

        std::memcpy(pixels, other.pixels, size);
    }
}

Texture::Texture(Texture &&other) noexcept : pixels(nullptr), width(other.width), height(other.height),
    format(other.format)
{
    pixels = other.pixels;
    other.pixels = nullptr;
}

Texture::Texture(Format format, int width, int height, void* data) {
    this->format = format;
    this->width = width;
    this->height = height;

    if (data != nullptr) {
        std::size_t size(width * height);
        switch (format) {
            case Format::RED:
                this->pixels = new unsigned char[size];
                break;
            case Format::RGBA:
                this->pixels = new uint32_t[size];
                break;
        }

        std::memcpy(pixels, data, size);
    }
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
    format = other.format;

   return *this;
}

Texture::~Texture() {
    if (pixels != nullptr)
        delete[] reinterpret_cast<char*>(pixels);
}

void Texture::load_from_bmp(std::string path) {
    BmpReader bmp_reader;

    // TODO fix this
    // fucntion call allocates memory for pixels
    bmp_reader.open_file(path);

    size_t s {0};
    auto bitmap = std::unique_ptr<unsigned char>(nullptr);
    bmp_reader.read_to_buffer(bitmap);

    pixels = bitmap.get();

    // Ownership is released because this class manages the pointer for a bit extra performance
    bitmap.release();

    if (bmp_reader.get_bits_per_pixel() == 32) {
        format = Format::RGBA;
    } else {
        format = Format::RGB;
    }

    width = bmp_reader.get_width();
    height = bmp_reader.get_height();
}

Pixel Texture::get_pixel(int x_pos, int y_pos, float light_amount) const {
    Pixel r = {
        .value = get_pixel_value(x_pos, y_pos),
    };

    r.rgba.blue *= light_amount;
    r.rgba.red *= light_amount;
    r.rgba.green *= light_amount;

    return r;
}

Pixel Texture::get_pixel(int x_pos, int y_pos) const {
    return {
        .value = get_pixel_value(x_pos, y_pos),
    };

}

inline uint32_t Texture::get_pixel_value(int x_pos, int y_pos) const {
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

Texture Texture::from_section(Rect src) {
    uint32_t *data = new uint32_t[src.width * src.height];
    auto pixels = static_cast<uint32_t*>(this->pixels);

    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++)
            data[src.width * y + x] = get_pixel_value(x + src.x_pos, y + src.y_pos);

    return Texture(Format::RGBA, src.width, src.height, data);
}
