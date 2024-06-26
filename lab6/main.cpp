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

std::string ReadFileContent(const fs::path& path_to_file) {
    if (!fs::exists(path_to_file)) {
        throw std::invalid_argument("Filesystem object by path " + path_to_file.string() + " is not exists!");
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

std::set<std::string> GetFilesContentFromDirectory ( fs::path& path_to_directory){
    std::set<std::string> files_contents;
    
    for(const auto& entry: fs::directory_iterator(path_to_directory)){
        if(!fs::exists(entry.path())){
            throw std::invalid_argument("Filesystem object by path " + entry.path().string() + " is not exists!");
        }
        if(fs::is_directory(entry.path())){
            continue;  
        }
        if(fs::is_regular_file(entry.path())){
            files_contents.insert(ReadFileContent(entry.path()));
        }   
    }
    return files_contents;
}

int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));
        CheckInputPath(fs::path(argv[argc - 2]));
        fs::path directory_in = fs::path(argv[argc - 1]);
        fs::path directory_out = fs::path(argv[argc - 2]);
        std::set<std::string> files_content = GetFilesContentFromDirectory(directory_out);
        for(const auto& entry: fs::directory_iterator(directory_in)){
            const auto path_in_dir_out = fs::path(directory_out.string())  /  entry.path();
            std::string content = ReadFileContent(entry.path());
            if(files_content.find(content) == files_content.end()){
                fs::copy_file(entry.path(), path_in_dir_out, fs::copy_options::overwrite_existing);
                std::cout << "File " << entry.path().filename().string() << " is in " << path_in_dir_out.string() << std::endl;
                files_content.insert(content);
            }
        }
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

