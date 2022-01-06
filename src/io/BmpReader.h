#pragma once

#include <string>
#include <fstream>
#include <memory>

using std::string;

class BmpReader {
public:
    BmpReader();
    ~BmpReader();

    bool open_file(string path);
    size_t read_to_buffer(std::unique_ptr<unsigned char> &bitmap);
    void* read_file(string path);
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
