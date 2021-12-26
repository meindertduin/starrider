#include "Texture.h"
#include "Core.h"

Texture::Texture() {
}

Texture::~Texture() {
    if (p_bitmap != nullptr)
        delete p_bitmap;
}


void Texture::load_from_bmp(std::string path) {
    p_bitmap = new Bitmap(path);
    width = p_bitmap->width;
    height = p_bitmap->height;
}

Pixel Texture::get_pixel(int x_pos, int y_pos, float light_amount) const {
    Pixel r = {
        .value = p_bitmap->get_value(x_pos, y_pos)
    };

    r.rgba.blue *= light_amount;
    r.rgba.red *= light_amount;
    r.rgba.green *= light_amount;

    return r;
}

Pixel Texture::get_pixel(int x_pos, int y_pos) const {
    return {
        .value = p_bitmap->get_value(x_pos, y_pos)
    };
}
