#include "Texture.h"
#include "Core.h"

Texture::Texture() {
}

Texture::~Texture() {
    if (m_bitmap.pixels != nullptr) {
        delete static_cast<char*>(m_bitmap.pixels);
    }
}

void Texture::load_from_bitmap(Format format, int width, int height, void* data) {
    m_bitmap = Bitmap(format, width, height, data);

    width = m_bitmap.width;
    height = m_bitmap.height;
}

void Texture::load_from_bmp(std::string path) {
    m_bitmap = Bitmap(path);
    width = m_bitmap.width;
    height = m_bitmap.height;
}

void Texture::load_from_bitmap(Bitmap &&bitmap) {
    m_bitmap = bitmap;
    width = m_bitmap.width;
    width = m_bitmap.height;
}

Pixel Texture::get_pixel(int x_pos, int y_pos, float light_amount) const {
    Pixel r = {
        .value = m_bitmap.get_value(x_pos, y_pos)
    };

    r.rgba.blue *= light_amount;
    r.rgba.red *= light_amount;
    r.rgba.green *= light_amount;

    return r;
}

Pixel Texture::get_pixel(int x_pos, int y_pos) const {
    return {
        .value = m_bitmap.get_value(x_pos, y_pos),
    };
}

Texture* Texture::from_section(Rect src) {
    Texture *r = new Texture();
    uint32_t *data = new uint32_t[src.width * src.height];
    auto pixels = static_cast<uint32_t*>(m_bitmap.pixels);

    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++)
            data[src.width * y + x] = m_bitmap.get_value(x + src.x_pos, y + src.y_pos);

    r->load_from_bitmap(std::move(Bitmap(Format::RGBA, src.width, src.height, data)));

    return r;
}
