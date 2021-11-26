#pragma once

#include <string>
#include <fstream>

using std::string;

class BmpReader {
public:
    BmpReader();
    ~BmpReader();

    size_t read_file(string path);
private:
    struct BmpHeader {
        uint32_t size;
        uint32_t pix_array_offset;
        uint32_t width;
        uint32_t height;
        u_int16_t bit_per_pixel;
    } m_header;

    char* p_pix_array;

    std::ifstream m_ifs;

    void read_header();
};
