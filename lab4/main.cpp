#include <iostream>
#include "json.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <time.h>
#include <ctime>
//variant 4
/*
    Given task: 
    Получить значения даты и времени, соответствующих концу
    прошлого года (23:59:59 31 декабря)
*/

namespace fs = std::filesystem;
using json = nlohmann::json;

void CheckArgumentsAmount(int arguments_amount){
    if(arguments_amount != 2){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - 2");
    }
}


void CheckInputPath (const fs::path& path_to_filesystem_object){
    if(!fs::exists(path_to_filesystem_object)){
        throw std::invalid_argument("Invalid input path: " + path_to_filesystem_object.string());
    }
}

std::size_t Size (const fs::path& path_to_filesystem_object){
    if(!fs::exists(path_to_filesystem_object)){
            throw std::invalid_argument("File or directory does not exists path: " + path_to_filesystem_object.string());
    }
    if(fs::is_regular_file(path_to_filesystem_object)){
        return fs::file_size(path_to_filesystem_object);
    }else if(fs::is_directory(path_to_filesystem_object)){
        std::size_t size = 0;
        for (const auto& entry : fs::directory_iterator(path_to_filesystem_object)) {
            if(!fs::exists(entry.path())) {
                throw std::invalid_argument("File or directory does not exists path: " + entry.path().string());
            }
            size += Size(entry.path());
        }
        return size;
    }
}

json GetRegularFileInfo (const fs::path& path_to_file){
    json info;
    info["type"] = "regular file";
    info["size"] = Size(path_to_file);
    info["full name"] = path_to_file.filename().string();
    info["name wizout extension"] = path_to_file.filename().stem().string();
    info["extension"] = path_to_file.extension().string();
    return info;
}

json GetDirectoryInfo (const fs::path& path_to_directory){
    json info;
    info["type"] = "directory";
    info["size"] = Size(path_to_directory);
    info["name"] = path_to_directory.filename().string();
    int amount_of_files = 0;
    int amount_of_directories = 0;
    for(const auto& entry : fs::directory_iterator(path_to_directory)){
        if(fs::is_regular_file(entry.path()))
            amount_of_files++;
        else if(fs::is_directory(entry.path()))
            amount_of_directories++;
    }
    info["files_amount"] = amount_of_files;
    info["directories_amount"] = amount_of_directories;
    return info;
}

json GetFileInfo (const fs::path& path_to_file){
    if(fs::is_regular_file(path_to_file)){
        return GetRegularFileInfo(path_to_file);
    }else if(fs::is_directory(path_to_file)){
        return GetDirectoryInfo(path_to_file);
    }
}

int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));
        
        json info = GetFileInfo(fs::path(argv[argc - 1]));
        std::cout << info.dump(4) << std::endl;
        std::ofstream out("info.json");
        out << info.dump(4);
        out.close();
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

