#include "BmpReader.h"

BmpReader::BmpReader() {}

BmpReader::~BmpReader() {
    if (m_ifs.is_open()) {
        m_ifs.close();
    }

    delete[] p_pix_array;
}

size_t BmpReader::read_file(string path) {
    m_ifs = std::ifstream(path, std::ios::in | std::ios::binary);

    if (!m_ifs.is_open()) {
        // TODO: throw an error
    }

    read_header();

    printf("%d, %d\n", m_header.width, m_header.bit_per_pixel);

    return m_header.size;
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
}
