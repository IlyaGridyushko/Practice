#include "FilesStorage.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

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

void FilesStorage::InitStorage(){
    if(!fs::exists(path_to_directory_)){
        throw std::invalid_argument("Filesystem object by path " + path_to_directory_.string() + " is not exists!");
    } else if(!fs::is_directory(path_to_directory_)){
        throw std::invalid_argument("Filesystem object by path " + path_to_directory_.string() + " is not directory!");
    }

    for(const auto& entry: fs::directory_iterator(path_to_directory_)){
        if(!fs::exists(entry.path())){
            throw std::invalid_argument("Filesystem object by path " + entry.path().string() + " is not exists!");
        }
        if(fs::is_directory(entry.path())){
            continue;  
        }
        if(fs::is_regular_file(entry.path())){
            files_content_storage_.insert(ReadFileContent(entry.path()));
        }   
    }
}

void FilesStorage::CopyFile(const fs::path& path_to_file){
    if(fs::is_regular_file(path_to_file)){
       std::string content = ReadFileContent(path_to_file);
       if(files_content_storage_.find(content) == files_content_storage_.end()){
            const auto path_in_dir_out = path_to_directory_ / path_to_file.filename();
            fs::copy_file(path_to_file, path_in_dir_out, fs::copy_options::overwrite_existing);
            std::cout << "File " << path_to_file.filename().string() << " is in " << path_in_dir_out.string() << std::endl;
            files_content_storage_.insert(content);
       }
    }
    else{
        throw std::invalid_argument("Trying to copy not regular file: " + path_to_file.filename().string());
    }
}