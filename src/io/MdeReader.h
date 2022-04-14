#include <fstream>
#include <string>
#include <vector>
#include <memory>

struct MdeHeader {
    int version;

    int skin_size;

    int frame_size;

    int num_skins;
    int num_verts;
    int num_textcoords;
    int num_polys;
    int num_frames;

    int offset_skins;
    int offset_verts;
    int offset_textcoords;
    int offset_polys;
    int offset_frames;
    int offset_end;
};


std::ofstream& operator<<(std::ofstream& out, const MdeHeader &header);
std::ifstream& operator>>(std::ifstream& in, const MdeHeader &header);

struct MdePoly {
    unsigned short v_index[3];
    unsigned short t_index[3];

    // TODO: implement this one in the loader
    unsigned char normal_index;
};

struct MdeVert {
    float v[3];
};

struct MdeTextCoord {
    float u, v;
};

struct MdeFrame {
    MdeVert vlist[1];
};

struct MdeFile {
    MdeHeader header;

    std::unique_ptr<char[64]> skins;
    std::unique_ptr<MdeVert> verts;
    std::unique_ptr<MdeTextCoord> text_coords;
    std::unique_ptr<MdePoly> polys;
    std::unique_ptr<MdeFrame> frames;
};

class MdeReader {
public:
    MdeReader() = default;

    MdeReader(const MdeReader &other) = delete;
    MdeReader(MdeReader &&other) = delete;

    MdeReader& operator=(const MdeReader &other) = delete;
    MdeReader& operator=(MdeReader &&other) = delete;

    ~MdeReader();

    bool read_file(std::string path, MdeFile &result);
private:
    std::ifstream m_ifs;
};




