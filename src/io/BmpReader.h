#pragma once

#include <string>
#include <fstream>

using std::string;

class BmpReader {
public:
    BmpReader();
    ~BmpReader();

    size_t read_file(string path, void *&bitmap);
    uint32_t get_width();
    uint32_t get_height();
private:
    struct BmpHeader {
        uint32_t size;
        uint32_t pix_array_offset;
        uint32_t width;
        uint32_t height;
        u_int16_t bit_per_pixel;
        uint32_t bitmap_size;
    } m_header;

    std::ifstream m_ifs;

    void read_header();
    uint32_t* row(int row, uint32_t* bitmap);
};
