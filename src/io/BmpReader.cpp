#include "BmpReader.h"

BmpReader::BmpReader() {}

BmpReader::~BmpReader() {
    if (m_ifs.is_open()) {
        m_ifs.close();
    }
}

size_t BmpReader::read_file(string path, uint32_t *bitmap) {
    m_ifs = std::ifstream(path, std::ios::in | std::ios::binary);

    if (!m_ifs.is_open()) {
        // TODO: throw an error
    }

    read_header();
    bitmap = new uint32_t[m_header.bitmap_size / (m_header.bit_per_pixel / 8)];

    m_ifs.seekg(m_header.pix_array_offset, std::ios_base::beg);
    m_ifs.read(reinterpret_cast<char*>(bitmap), m_header.bitmap_size);

    return m_header.bitmap_size;
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
