#include "Texture.h"
#include "Core.h"

Texture::Texture() {

}

void Texture::load_from_bmp(std::string path) {
    m_bitmap = Bitmap(path);
}
