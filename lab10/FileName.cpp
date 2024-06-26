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
#include "FileName.H"

namespace fs = std::filesystem;

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


bool FileName::IsRemoveRequired() const{
    if((year % 5 == 0) && (day % 5 == 0)){
        return true;
    }else{
        return false;
    }
}

void FileName::Parse(){
    std::string file_name_without_extension = path_to_current_file.filename().stem().string();
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
        throw std::invalid_argument("Invalid file format: " + path_to_current_file.string());
    }
    year = (std::uint16_t)std::stoi(splitted_file_name[0]);
    month = (std::uint16_t)std::stoi(splitted_file_name[1]);
    day = (std::uint16_t)std::stoi(splitted_file_name[2]);
    only_name = splitted_file_name[3];
}