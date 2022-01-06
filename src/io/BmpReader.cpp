#include "BmpReader.h"
#include <iostream>

BmpReader::BmpReader() {}

BmpReader::~BmpReader() {
    if (m_ifs.is_open()) {
        m_ifs.close();
    }
}

bool BmpReader::open_file(string path) {
    m_ifs = std::ifstream(path, std::ios::in | std::ios::binary);

    if (!m_ifs.is_open()) {
        std::cerr << "BitReader::read_file() Error: file "
            << path << " not found!" << std::endl;
        return false;
    }

    read_header();

    return true;
};

size_t BmpReader::read_to_buffer(std::unique_ptr<unsigned char> &bitmap) {
    auto raw = new uint32_t[m_header.bitmap_size / (m_header.bit_per_pixel / 8)];
    bitmap.reset(reinterpret_cast<unsigned char*>(raw));

    m_ifs.seekg(m_header.pix_array_offset, std::ios_base::beg);

    for (int i = 0; i < m_header.height; i++) {
        // reading the rows in inverse order, so that the bitmap of the image isn't inverted.
        uint32_t* row_ptr = row(m_header.height - i - 1, reinterpret_cast<uint32_t*>(bitmap.get()));
        m_ifs.read(reinterpret_cast<char*>(row_ptr), 4 * m_header.width);
    }

    return m_header.bitmap_size;
}

void* BmpReader::read_file(string path) {
    auto bitmap = new uint32_t[m_header.bitmap_size / (m_header.bit_per_pixel / 8)];

    m_ifs.seekg(m_header.pix_array_offset, std::ios_base::beg);

    for (int i = 0; i < m_header.height; i++) {
        // reading the rows in inverse order, so that the bitmap of the image isn't inverted.
        uint32_t* row_ptr = row(m_header.height - i - 1, reinterpret_cast<uint32_t*>(bitmap));
        m_ifs.read(reinterpret_cast<char*>(row_ptr), 4 * m_header.width);
    }

    return bitmap;
}



void BmpReader::read_header() {
    m_ifs.seekg(0x02, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(&m_header.size), sizeof(m_header.size));

    m_ifs.seekg(0x0A, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(&m_header.pix_array_offset), sizeof(m_header.pix_array_offset));

    m_ifs.seekg(0x12, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(&m_header.width), sizeof(m_header.width));
    m_ifs.read(reinterpret_cast<char*>(&m_header.height), sizeof(m_header.height));

    m_ifs.seekg(0x1C, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(&m_header.bit_per_pixel), sizeof(m_header.bit_per_pixel));

    m_ifs.seekg(0x22, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(&m_header.bitmap_size), sizeof(m_header.bitmap_size));
}

uint32_t BmpReader::get_width() {
    return m_header.width;
}

uint32_t BmpReader::get_height() {
    return m_header.height;
}

uint32_t* BmpReader::row(int row, uint32_t* bitmap) {
    return &bitmap[m_header.width * row];
}
