#pragma

#include <string>
#include "Core.h"

class Texture {
public:
    Texture();
    void test_load();
    void load_from_bmp(std::string path);
private:
    int width;
    int height;

    Bitmap m_bitmap;
};


