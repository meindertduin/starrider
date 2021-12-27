#include "Texture.h"
#include "Core.h"

Texture::Texture() {
}

Texture::~Texture() {
    if (m_bitmap != nullptr)
        delete m_bitmap;
}

void Texture::load_from_bitmap(Format format, int width, int height, void* data) {
    m_bitmap = new Bitmap(format, width, height, data);

    width = m_bitmap->width;
    height = m_bitmap->height;
}

void Texture::load_from_bmp(std::string path) {
    m_bitmap = new Bitmap(path);
    width = m_bitmap->width;
    height = m_bitmap->height;
}

void Texture::load_from_bitmap(Bitmap *bitmap) {
    m_bitmap = bitmap;
    width = m_bitmap->width;
    width = m_bitmap->height;
}

Pixel Texture::get_pixel(int x_pos, int y_pos, float light_amount) const {
    Pixel r = {
        .value = m_bitmap->get_value(x_pos, y_pos)
    };

    r.rgba.blue *= light_amount;
    r.rgba.red *= light_amount;
    r.rgba.green *= light_amount;

    return r;
}

Pixel Texture::get_pixel(int x_pos, int y_pos) const {
    return {
        .value = m_bitmap->get_value(y_pos, width - x_pos)
    };
}
