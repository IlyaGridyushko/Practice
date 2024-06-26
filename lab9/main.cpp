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
#include <chrono>

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

bool is_valid_date(int year, int month, int day) {
    try{
        if (month < 1 || month > 12 || day < 1) {
            return false;
        }
        std::chrono::year y{year};
        std::chrono::month m{month};
        std::chrono::day d{day};  
        std::chrono::year_month_day date{y / m / d};
        return date.ok();
    }catch(...){
        return false;
    }
}

std::filesystem::path GetPathToMove(const fs::path& path_to_file){
    std::string file_name_without_extension = path_to_file.filename().stem().string();
    char delemitr = '_';
    std::vector<std::string> splitted_file_name;
    while(file_name_without_extension.find(delemitr)!= std::string::npos){
        std::size_t pos = file_name_without_extension.find(delemitr);
        std::string token = file_name_without_extension.substr(0, pos);
        splitted_file_name.push_back(token);
        file_name_without_extension.erase(0, pos + 1);
    }
    splitted_file_name.push_back(file_name_without_extension);
    if((splitted_file_name.size() != 4) ||
       (!is_valid_date(std::stoi(splitted_file_name[0]), std::stoi(splitted_file_name[1]), std::stoi(splitted_file_name[2]))) ||
       (splitted_file_name[1].size()!= 2) ||
       (splitted_file_name[2].size()!= 2)){
        throw std::invalid_argument("Invalid file format: " + path_to_file.string());
    }
    return path_to_file.parent_path() / fs::path(splitted_file_name[0])  /
    fs::path(splitted_file_name[1])   / fs::path(splitted_file_name[2])  / 
    fs::path(splitted_file_name[3] + path_to_file.extension().string());
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
        CheckInputDirectoryPath(fs::path(argv[argc - 1]));
        fs::path directory = fs::path(argv[argc - 1]);
        for(int i = 2;i < argc;i++){
            CheckInputFilePath(fs::path(argv[i]));
            CheckIfFileInDirectory(fs::path(argv[i]), directory);
        }
        for(int i = 2;i < argc;i++){
            Move(directory / fs::path(argv[i]));
        }
        return 0;
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

