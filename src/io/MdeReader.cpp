#include <iostream>
#include <iterator>

#include "MdeReader.h"

std::ofstream& operator<<(std::ofstream& out, const MdeHeader &header) {
    out.write((char*) &header.version, sizeof(header.version));
    out.write((char*) &header.skin_size, sizeof(header.skin_size));
    out.write((char*) &header.frame_size, sizeof(header.frame_size));
    out.write((char*) &header.num_skins, sizeof(header.num_skins));
    out.write((char*) &header.num_verts, sizeof(header.num_verts));
    out.write((char*) &header.num_textcoords, sizeof(header.num_textcoords));
    out.write((char*) &header.num_polys, sizeof(header.num_polys));
    out.write((char*) &header.num_frames, sizeof(header.num_frames));

    out.write((char*) &header.offset_skins, sizeof(header.offset_skins));
    out.write((char*) &header.offset_verts, sizeof(header.offset_verts));
    out.write((char*) &header.offset_polys, sizeof(header.offset_polys));
    out.write((char*) &header.offset_textcoords, sizeof(header.offset_textcoords));
    out.write((char*) &header.offset_frames, sizeof(header.offset_frames));
    out.write((char*) &header.offset_end, sizeof(header.offset_end));

    return out;
}

std::ifstream& operator>>(std::ifstream& in, const MdeHeader &header) {
    in.read((char*) &header.version, sizeof(header.version));
    in.read((char*) &header.skin_size, sizeof(header.skin_size));
    in.read((char*) &header.frame_size, sizeof(header.frame_size));
    in.read((char*) &header.num_skins, sizeof(header.num_skins));
    in.read((char*) &header.num_verts, sizeof(header.num_verts));
    in.read((char*) &header.num_textcoords, sizeof(header.num_textcoords));
    in.read((char*) &header.num_polys, sizeof(header.num_polys));
    in.read((char*) &header.num_frames, sizeof(header.num_frames));

    in.read((char*) &header.offset_skins, sizeof(header.offset_skins));
    in.read((char*) &header.offset_verts, sizeof(header.offset_verts));
    in.read((char*) &header.offset_polys, sizeof(header.offset_polys));
    in.read((char*) &header.offset_textcoords, sizeof(header.offset_textcoords));
    in.read((char*) &header.offset_frames, sizeof(header.offset_frames));
    in.read((char*) &header.offset_end, sizeof(header.offset_end));

    return in;
}

MdeReader::~MdeReader() {
    if (m_ifs.is_open())
        m_ifs.close();
}

bool MdeReader::read_file(std::string path, MdeFile &result) {
    m_ifs = std::ifstream(path, std::ios::in | std::ios::binary);

    if (!m_ifs.is_open()) {
        std::cerr << "BitReader::read_file() Error: file "
            << path << " not found!" << std::endl;
        return false;
    }

    MdeHeader header;
    m_ifs >> header;

    // Read the skins
    m_ifs.seekg(header.offset_skins, std::ios_base::beg);

    result.skins = std::unique_ptr<char[64]>(new char[header.num_skins][64]);

    m_ifs.read(reinterpret_cast<char*>(result.skins.get()), sizeof(char) * 64 * header.num_skins);

    // Read the vert data
    m_ifs.seekg(header.offset_verts, std::ios_base::beg);

    int verts_amount = header.num_verts * header.num_frames;
    result.verts = std::unique_ptr<MdeVert>(new MdeVert[verts_amount]);

    m_ifs.read(reinterpret_cast<char*>(result.verts.get()), sizeof(MdeVert) * verts_amount);

    // Read the texture coordinates
    m_ifs.seekg(header.offset_textcoords, std::ios_base::beg);
    result.text_coords = std::unique_ptr<MdeTextCoord>(new MdeTextCoord[header.num_textcoords]);

    m_ifs.read(reinterpret_cast<char*>(result.text_coords.get()), sizeof(MdeTextCoord) * header.num_textcoords);

    // Read polys data
    m_ifs.seekg(header.offset_polys, std::ios_base::beg);

    int polys_amount = header.num_polys * header.num_frames;
    result.polys = std::unique_ptr<MdePoly>(new MdePoly[polys_amount]);

    m_ifs.read(reinterpret_cast<char*>(result.polys.get()), sizeof(MdePoly) * polys_amount);

    // Read frame data
    m_ifs.seekg(header.offset_textcoords, std::ios_base::beg);
    result.frames = std::unique_ptr<MdeFrame>(new MdeFrame[header.num_frames]);

    m_ifs.read(reinterpret_cast<char*>(result.frames.get()), sizeof(MdeFrame) * header.num_frames);

    return true;
}
