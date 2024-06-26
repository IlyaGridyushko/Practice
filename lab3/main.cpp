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
    if(arguments_amount > 2){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - 1 / 2");
    }
}


void CheckInputPath (const fs::path& path_to_filesystem_object){
    if(!fs::exists(path_to_filesystem_object)){
        throw std::invalid_argument("Invalid input path: " + path_to_filesystem_object.string());
    }
    if(!fs::is_regular_file(path_to_filesystem_object)){
        throw std::invalid_argument("Not regular file: " + path_to_filesystem_object.string());
    }
    if(!(path_to_filesystem_object.extension() == fs::path(".json"))){
        throw std::invalid_argument("Not json file: " + path_to_filesystem_object.string());
    }
}

tm GetRequiredDateTime(const tm* current_time){
    struct tm new_time;
    new_time.tm_sec = 59;
    new_time.tm_min = 59;
    new_time.tm_hour = 23;
    new_time.tm_mday = 31;
    new_time.tm_mon = 11;
    new_time.tm_year = current_time->tm_year - 1; 
    new_time.tm_isdst = current_time->tm_isdst;
    mktime(&new_time);
    return new_time;
}

json TmToJson(tm time_struct){
    json j;
    j["tm_sec"]   = time_struct.tm_sec;  
    j["tm_min"]   = time_struct.tm_min;   
    j["tm_hour"]  = time_struct.tm_hour; 
    j["tm_mday"]  = time_struct.tm_mday;  
    j["tm_mon"]   = time_struct.tm_mon;  
    j["tm_year"]  = time_struct.tm_year;
    j["tm_wday"]  = time_struct.tm_wday; 
    j["tm_yday"]  = time_struct.tm_yday; 
    j["tm_isdst"] = time_struct.tm_isdst; 
    return j;
}

tm JsonToTm(json time_json){
    if(!time_json.contains("tm_sec")   || !time_json.contains("tm_min")  || 
       !time_json.contains("tm_hour")  || !time_json.contains("tm_mday") || 
       !time_json.contains("tm_mon")   || !time_json.contains("tm_year") ||
       !time_json.contains("tm_wday")  || !time_json.contains("tm_yday") ||
       !time_json.contains("tm_isdst") || time_json.size() != 9){
            throw std::invalid_argument("json file errror: wrong format");
        }
    struct tm time_struct{0};
    time_struct.tm_sec = time_json["tm_sec"];
    time_struct.tm_min = time_json["tm_min"];
    time_struct.tm_hour = time_json["tm_hour"];
    time_struct.tm_mday = time_json["tm_mday"];
    time_struct.tm_mon = time_json["tm_mon"];
    time_struct.tm_year = time_json["tm_year"];
    time_struct.tm_wday = time_json["tm_wday"];
    time_struct.tm_yday = time_json["tm_yday"];
    time_struct.tm_isdst = time_json["tm_isdst"];
    return time_struct;
}


int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);

        if(argc == 1){
            std::time_t currentTime = std::time(nullptr);
            std::tm* localTime = std::localtime(&currentTime);
            tm required_time = GetRequiredDateTime(localTime);
            json required_time_json = TmToJson(required_time);
            std::cout << required_time_json.dump(4) << std::endl;
            std::ofstream output_file("required_time.json");
            output_file << required_time_json.dump(4);
            output_file.close();
        }else{
            CheckInputPath(fs::path(argv[1]));
            std::ifstream file(argv[1]);
            json data =  json::parse(file);
            tm current_time = JsonToTm(data);
            tm required_time = GetRequiredDateTime(&current_time);
            json required_time_json = TmToJson(required_time);
            std::cout << required_time_json.dump(4) << std::endl;
            std::ofstream output_file("required_time.json");
            output_file << required_time_json.dump(4);
            output_file.close();
        }
    }catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

