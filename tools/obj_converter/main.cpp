#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <exception>
#include <algorithm>
#include <fstream>
#include <iterator>

#include "../../src/io/ObjReader.h"
#include "../../src/io/MdeReader.h"

using std::string;
using std::vector;

int convert_obj_files(std::string dir, std::string output_path);
bool ends_width(const std::string &full_string, const std::string &ending);
std::vector<std::string> get_obj_file_paths(std::string path);

bool read_obj_files(vector<string> file_paths, vector<vector<MdeVert>> &total_verts, vector<vector<MdePoly>> &total_polys, vector<MdeTextCoord> &text_coords, vector<MdeFrame> &frames);
bool write_mde_data(string output_path, vector<vector<MdeVert>> &total_verts, vector<vector<MdePoly>> &total_polys, vector<MdeTextCoord> &text_coords, vector<MdeFrame> &frames);

int main(int argc, char *argv[]) {
    std::vector<std::string> argument_inputs;

    for (int i = 0; i < argc; i++)
        argument_inputs.push_back(std::string(argv[i]));

    std::string input_dir;
    std::string output_path;

    for (int i = 1; i < argument_inputs.size(); i++) {
        if (argument_inputs[i] == "-i") {
            input_dir = argument_inputs[i + 1];
        }

        if (argument_inputs[i] == "-o") {
            output_path = argument_inputs[i + 1];
        }
    }

    convert_obj_files(input_dir, output_path);

    return 0;
}


int convert_obj_files(std::string path, std::string output_path) {
    auto file_paths = get_obj_file_paths(path);

    std::vector<std::vector<MdeVert>> total_verts;
    std::vector<std::vector<MdePoly>> total_polys;
    std::vector<MdeTextCoord> text_coords;
    std::vector<MdeFrame> frames;

    read_obj_files(file_paths, total_verts, total_polys, text_coords, frames);
    write_mde_data(output_path, total_verts, total_polys, text_coords, frames);

    return 0;
}

bool read_obj_files(vector<string> file_paths, vector<vector<MdeVert>> &total_verts,
        vector<vector<MdePoly>> &total_polys, vector<MdeTextCoord> &text_coords, vector<MdeFrame> &frames)
{
    ObjReader reader;

    auto i = 0;
    bool first = true;
    for (const auto &file_path : file_paths) {
        if (!reader.read_file(file_path)) {
            return false;
        }

        std::vector<MdeVert> verts;
        for (const auto &vert : reader.m_vertices)
            verts.push_back(MdeVert {vert.x, vert.y, vert.z});

        total_verts.push_back(verts);

        if (first) {
            for (const auto &text_coord : reader.m_tex_coords)
                text_coords.push_back(MdeTextCoord {text_coord.x, text_coord.y });

            first = false;
        }

        std::vector<MdePoly> polys;
        for (int i = 0; i < reader.m_indices.size(); i+= 3) {
            MdePoly poly;

            for (int j = 0; j < 3; j++) {
                auto current_index = reader.m_indices[i + j];

                poly.v_index[j] = current_index.vertex_index;

                if (reader.has_tex_coords) {
                    poly.t_index[j] = current_index.tex_coord_index;
                }

                // TODO also set the normal index
                if (reader.has_normal_indices) {
                    poly.normal_index = current_index.normal_index;
                }
            }

            polys.push_back(poly);
        }

        total_polys.push_back(polys);
        frames.push_back(MdeFrame{});

        std::cout << file_path << std::endl;

        i++;
    }

    return true;
}

bool write_mde_data(string output_path, vector<vector<MdeVert>> &total_verts, vector<vector<MdePoly>> &total_polys,
        vector<MdeTextCoord> &text_coords, vector<MdeFrame> &frames)
{
    MdeHeader header;

    header.version = 1;
    header.skin_size = 0;
    header.frame_size = sizeof(MdeFrame);

    header.num_skins = 0;
    header.num_verts = total_verts[0].size();
    header.num_textcoords = text_coords.size();
    header.num_polys = total_polys[0].size();
    header.num_frames = total_verts.size();

    header.offset_skins = sizeof(MdeHeader);
    header.offset_verts = header.num_skins * sizeof(char) * 64;
    header.offset_polys = header.num_textcoords * sizeof(MdeTextCoord);
    header.offset_textcoords = header.num_verts * sizeof (MdeVert) * header.num_frames;
    header.offset_frames = header.num_polys * sizeof(MdePoly) * header.num_frames;
    header.offset_end = header.frame_size * header.num_frames;

    std::ofstream fs(output_path);
    fs << header;

    // TODO write out the skin paths

    for (auto &verts : total_verts) {
        fs.write(reinterpret_cast<char*>(&verts[0]), verts.size() * sizeof(MdeVert));
    }

    fs.write(reinterpret_cast<char*>(&text_coords[0]), text_coords.size() * sizeof(MdeTextCoord));

    for (auto &polys : total_polys) {
        fs.write(reinterpret_cast<char*>(&polys[0]), polys.size() * sizeof(MdePoly));
    }

    fs.write(reinterpret_cast<char*>(&frames[0]), frames.size() * sizeof(MdeFrame));

    if (!fs.is_open()) {
        return false;
    }

    fs.close();

    return true;
}

std::vector<std::string> get_obj_file_paths(std::string path) {
    std::vector<std::string> file_paths;

    try {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            auto file_path = entry.path().string();

            if (ends_width(file_path, ".obj")) {
                file_paths.push_back(file_path);
            }
        }
    } catch(std::filesystem::filesystem_error error) {
        printf("Could not open directory. Are you sure the directory exists?\n");
    }

    std::sort(file_paths.begin(), file_paths.end(), [](const std::string &a, const std::string &b) -> bool {
        auto a_num = a.substr(a.size() - 10, a.size() -4).substr(0, 6);
        auto b_num = b.substr(b.size() -10, b.size() - 4).substr(0, 6);

        return std::stoi(a_num) < std::stoi(b_num);
    });

    return file_paths;
}

bool ends_width(const std::string &full_string, const std::string &ending) {
    if (full_string.length() >= ending.length()) {
        return (0 == full_string.compare (full_string.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
