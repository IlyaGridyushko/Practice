#if __cplusplus < 202002L
#error "C++20 or higher is required"
#endif
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>
#include "FileName.H"

namespace fs = std::filesystem;

void CheckArgumentsAmount(int arguments_amount){
    if(arguments_amount < 3){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - >= 3");
    }
}


void CheckInputDirectoryPath (const fs::path& path_to_filesystem_object){
    if(!fs::exists(path_to_filesystem_object)){
        throw std::invalid_argument("Invalid input path: " + path_to_filesystem_object.string());
    }
    if(!fs::is_directory(path_to_filesystem_object)){
        throw std::invalid_argument("Is not a directory:: " + path_to_filesystem_object.string());
    }
}

void CheckInputFilePath (const fs::path& path_to_file){
    if(!fs::exists(path_to_file)){
        throw std::invalid_argument("Invalid input path: " + path_to_file.string() + ". - file does not exists");
    }
    if(!fs::is_regular_file(path_to_file)){
        throw std::invalid_argument("Is not a regular file:: " + path_to_file.string());
    }

}

void CheckIfFileInDirectory(const fs::path& path_to_file, const fs::path& path_to_directory){
    fs::path full_path_to_file = path_to_directory / path_to_file.filename();
    if (!fs::exists(full_path_to_file) && fs::is_regular_file(full_path_to_file)) {
        throw std::invalid_argument("File " + path_to_file.filename().string()
         + " does not exists in directory: " + path_to_directory.string());
    }
}



std::filesystem::path GetPathToMove(const fs::path& path_to_file){
    FileName file_name(path_to_file.filename().string());
    file_name.Parse();
    return path_to_file.parent_path() / fs::path(std::to_string(file_name.year)) /
        fs::path(std::to_string(file_name.month)) / fs::path(std::to_string(file_name.day)) / 
        fs::path(file_name.only_name + path_to_file.extension().string());
}

void Move(const fs::path& path_to_file){
    try{
        fs::path target_path = GetPathToMove(path_to_file);
        fs::create_directories(target_path.parent_path());
        fs::rename(path_to_file, target_path);
        std::cout << "File: " + path_to_file.string() +  " moved successfully to: " << target_path.string() << std::endl;
    }catch(std::exception& e){
        throw std::runtime_error("(File: " + path_to_file.string() + " wasnt move) beause: " + e.what() + '\n');
    }
}

int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputDirectoryPath(fs::path(argv[1]));
        fs::path directory = fs::path(argv[1]);
        for(int i = 2;i < argc;i++){
            CheckInputFilePath(directory / fs::path(argv[i]));
            CheckIfFileInDirectory(fs::path(argv[i]), directory);
        }
        
        for(const auto& entry : fs::directory_iterator(directory)){
            if(!fs::exists(entry.path())){
                throw std::runtime_error("File " + entry.path().filename().string() + " does not exists");
            }
            if(fs::is_regular_file(entry.path())){
                FileName file_name(entry.path().filename());
                file_name.Parse();
                if(file_name.IsRemoveRequired()){
                    fs::remove(entry.path());
                    std::cout << "File " + entry.path().filename().string() + " was removed successfully" << std::endl;
                }
            }
        }
        for(int i = 2;i < argc;i++){
            if(fs::exists(directory / fs::path(argv[i]))){
                Move(directory / fs::path(argv[i]));
            }
        return 0;
        }
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

