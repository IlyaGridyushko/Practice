#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>
#include "FilesStorage.h"

namespace fs = std::filesystem;

void CheckArgumentsAmount(int arguments_amount){
    if(arguments_amount != 3){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - 3");
    }
}


void CheckInputPath (const fs::path& path_to_filesystem_object){
    if(!fs::exists(path_to_filesystem_object)){
        throw std::invalid_argument("Invalid input path: " + path_to_filesystem_object.string());
    }
    if(!fs::is_directory(path_to_filesystem_object)){
        throw std::invalid_argument("Is not a directory:: " + path_to_filesystem_object.string());
    }
}


int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));
        CheckInputPath(fs::path(argv[argc - 2]));
        fs::path directory_in = fs::path(argv[argc - 2]);
        fs::path directory_out = fs::path(argv[argc - 1]);
        FilesStorage storage(directory_out);
        storage.InitStorage();
        for(const auto entry : fs::directory_iterator(directory_in)){
            if(fs::is_regular_file(entry.path())){
                fs::path file_path = entry.path();
                storage.CopyFile(file_path);
            }
        }
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

