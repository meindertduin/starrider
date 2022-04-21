#include "MapReader.h"

#include <sstream>
#include <iostream>

bool MapReader::read_file(string path, MapFile &file) {
    m_ifs = std::ifstream(path);

    if (!m_ifs.is_open()) {
        return false;
    }

    string line;
    string first_token;

    std::stringstream ss;

    while(std::getline(m_ifs, line)) {
        ss.str("");
        ss.clear();

        ss << line;
        ss >> first_token;

        if (first_token == "d") {
            ss >> file.width >> file.farth;
            file.ter_codes = vector<TerrainTile>(file.width * file.farth);
            std::fill(file.ter_codes.begin(), file.ter_codes.end(), TerrainTile { 0 });
        } else if (first_token == "s") {
            read_section(ss, file);
        }
    }

    return true;
}

void MapReader::read_section(std::stringstream &ss, MapFile &file) {
    string sec_str;

    ss >> sec_str;

    if (sec_str == "ter") {
        m_cur_section = Section::Terrain;
    } else if (sec_str == "tex") {
        m_cur_section = Section::Texture;
    }

    string line;
    for (int i = 0; i < file.farth; i++) {
        std::getline(m_ifs, line);

        ss.str("");
        ss.clear();

        ss << line;

        unsigned short token;
        switch (m_cur_section) {
            case Section::Terrain:
                for (int j = 0; ss >> token; j++) {
                    file.ter_codes[j + file.width * i].height = token;
                }
                break;
            case Section::Texture:
                for (int j = 0; ss >> token; j++) {
                    file.ter_codes[j + file.width * i].terrain = token;
                }
                break;
        }
    }
}

MapReader::~MapReader() {
    m_ifs.close();
}
