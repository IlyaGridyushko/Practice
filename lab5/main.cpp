#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

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
    return 0;
}

namespace filesystem_object {
    struct Info {
        std::string name;   
        std::string type;   
        std::size_t size;   

        Info(const fs::path& path) {
            if (fs::is_directory(path)) {
                type = "directory";
                name = path.filename().string();
                size = Size(path); 
            } else if (fs::is_regular_file(path)) {
                name = path.stem().string(); 
                if (path.extension().empty()) {
                    type = "file without extension";
                } else {
                    type = path.extension().string();
                }
                size = Size(path);;
            } else {
                type = "unknown";
                size = 0;
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Info& info) {
            os << std::left << std::setfill(' ')
               << std::setw(50) << info.name
               << std::setw(20) << info.type
               << std::setw(20) << info.size;
            return os;
        }
    };
}

namespace directory_content{
    struct Info {
        std::string path_to_directory;
        std::size_t size;
        std::uint32_t files_amount;
        std::uint32_t directories_amount;

        Info(const fs::path& path_to_directory) {
            this->path_to_directory = path_to_directory.string();
            size = Size(path_to_directory);
            files_amount = 0;
            directories_amount = 0;
            for (const auto& entry : fs::directory_iterator(path_to_directory)) {
                if(!fs::exists(entry.path())) {
                    throw std::invalid_argument("File or directory does not exists path: " + entry.path().string());
                }
                if(fs::is_directory(entry.path())) {
                    directories_amount++;
                } else if(fs::is_regular_file(entry.path())) {
                    files_amount++;
                }
            }

        }

        friend std::ostream& operator<<(std::ostream& os, const Info& info) {
            os << std::left << std::setfill(' ')
               << std::setw(50) << "Path to Directory:" << std::setw(20) << info.path_to_directory << '\n'
               << std::setw(50) << "Size (bytes):" << std::setw(20) << info.size << '\n'
               << std::setw(50) << "Files Amount:" << std::setw(20) << info.files_amount << '\n'
               << std::setw(50) << "Directories Amount:" << std::setw(20) << info.directories_amount << '\n';
            return os;
        }
    };
}


int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));
        fs::path path = fs::path(argv[argc - 1]);
        filesystem_object::Info info(path);
        directory_content::Info info_directory(path);
        std::cout << info << '\n';
        std::cout << info_directory << '\n';
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

