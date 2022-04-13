#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <exception>

#include "../../src/io/ObjReader.h"

int convert_obj_files(std::string dir);

int main(int argc, char *argv[]) {
    std::vector<std::string> argument_inputs;

    for (int i = 0; i < argc; i++)
        argument_inputs.push_back(std::string(argv[i]));

    convert_obj_files(argument_inputs[1]);

    return 0;
}

int convert_obj_files(std::string path) {
    std::vector<std::string> file_paths;

    try {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            file_paths.push_back(entry.path().string());
    } catch(std::filesystem::filesystem_error error) {
        printf("Could not open directory. Are you sure the directory exists?\n");
    }

    return 0;
}
