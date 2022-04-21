#include <fstream>
#include <vector>
#include <string>

using std::string;
using std::vector;

struct TerrainTile {
    unsigned short terrain;
    unsigned short height;
};

struct MapFile {
    int width, farth;

    vector<TerrainTile> ter_codes;
};

class MapReader {
public:
    MapReader() = default;

    MapReader(const MapReader &other) = delete;
    MapReader(MapReader &&other) = delete;

    MapReader& operator=(const MapReader &other) = delete;
    MapReader& operator=(MapReader &&other) = delete;

    ~MapReader();

    bool read_file(string path, MapFile &file);
private:
    std::ifstream m_ifs;

    enum class Section {
        Terrain,
        Texture,
    } m_cur_section;

    void read_section(std::stringstream &ss, MapFile &file);
};
