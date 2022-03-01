#include "Texture.h"
#include "Core.h"
#include "RenderObject.h"
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
    this->width = width;
    this->height = height;

    if (data != nullptr) {
        auto size = width * height;
        this->pixels = new uint32_t[size];

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

   return *this;
}

Texture::~Texture() {
    delete[] reinterpret_cast<char*>(pixels);
}

void Texture::load_from_bmp(std::string path) {
    BmpReader bmp_reader;

    // fucntion call allocates memory for pixels
    bmp_reader.open_file(path);

    size_t s {0};
    auto bitmap = std::unique_ptr<unsigned char>(nullptr);
    bmp_reader.read_to_buffer(bitmap);

    pixels = reinterpret_cast<uint32_t*>(bitmap.get());

    // Ownership is released because this class manages the pointer for a bit extra performance
    bitmap.release();

    width = bmp_reader.get_width();
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


    height = bmp_reader.get_height();
}

Texture Texture::from_section(Rect src) {
    A565Color *data = new A565Color[src.width * src.height];

    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++)
            data[src.width * y + x] = get_pixel(x + src.x_pos, y + src.y_pos);

    return Texture(src.width, src.height, data);
}
