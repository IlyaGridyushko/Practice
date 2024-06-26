#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>

namespace fs = std::filesystem;

void CheckArgumentsAmount(int arguments_amount){
    if(arguments_amount != 2){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - 2");
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

std::string ReadFileContent(const fs::path& path_to_file) {
    if (!fs::exists(path_to_file)) {
        throw std::invalid_argument("Filesystem object by path " + path_to_file.string() + " is not exists!");
    }
    if(!fs::is_regular_file(path_to_file)){
        throw std::invalid_argument("Filesystem object by path " + path_to_file.string() + " is not regular file!");
    }
    std::ifstream file(path_to_file, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("File by path " + path_to_file.string() + " hasn’t been opened!");
    }
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string content(static_cast<size_t>(size), '\0');
    if (!file.read(content.data(), size)) {
        throw std::runtime_error("Error reading file content!");
    }

    return content;
}

std::size_t GetFileContentHash (const fs::path& path_to_file){
    std::string content = ReadFileContent(path_to_file);
    return std::hash<std::string>{}(content);
}

void RemoveDuplicatesFromDirectory(const fs::path& path_to_directory){
    std::set<std::size_t> hash_set;
    if(!fs::is_directory( path_to_directory)){
        throw std::invalid_argument("Is not a directory:: " +  path_to_directory.string());
    }
    for(const auto & entry : fs::directory_iterator(path_to_directory)){
        if(fs::is_directory(entry.path())){
            continue;
        }
        if(!fs::exists(entry.path())){
            throw std::invalid_argument("Filesystem object by path " + entry.path().string() + " is not exists!");
        }
        std::size_t hash = GetFileContentHash(entry.path());
        if(hash_set.find(hash) == hash_set.end()){
            hash_set.insert(hash);
        }else{
            fs::remove(entry.path());
        }
    }
}

int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));
        fs::path directory = fs::path(argv[argc - 1]);
        RemoveDuplicatesFromDirectory(directory);
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

