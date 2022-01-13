#pragma once

#include <string>
#include <fstream>
#include <memory>

using std::string;

class BmpReader {
public:
    BmpReader();

    bool open_file(string path);
    size_t read_to_buffer(std::unique_ptr<unsigned char> &bitmap);
    void* read_file(string path);

    uint32_t get_width() {
        return m_dib_header.width;
    }
    uint32_t get_height() const {
        return m_dib_header.height;
    }

    uint16_t get_bits_per_pixel() const {
        return m_dib_header.bits_per_pixel;
    }
private:
    struct BmpHeader {
        uint32_t size;
        uint32_t pix_array_offset;
    } m_header;

    struct DibHeader {
        uint32_t width;
        uint32_t height;
        u_int16_t bits_per_pixel;
        uint32_t bitmap_size;
    } m_dib_header;

    struct Int24 {
        unsigned int : 24;
    };

    std::ifstream m_ifs;

    inline void read_header();
    inline void read_dib_header();

    inline uint32_t* row(int row, uint32_t* bitmap);
};
