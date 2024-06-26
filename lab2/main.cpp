#include <iostream>
#include "json.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
//variant 4
/*
    Given task: 
    Удалить все пары ключ:значение, где значением является
    нечетное число
*/

namespace fs = std::filesystem;
using json = nlohmann::json;

void CheckArgumentsAmount(int arguments_amount){
    if(arguments_amount!= 2){
        throw std::invalid_argument("Invalid command line arguments amount: \ncurrent - " + std::to_string(arguments_amount) + "\nrequired - 2");
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

void ModifyJsonObject(nlohmann::json& json_object){
    std::vector<std::string> keys_to_remove;

    for(const auto& item: json_object.items()){
        if(item.value().is_object()){
            ModifyJsonObject(item.value());
        } else if(!item.value().is_number_integer()){
            throw std::invalid_argument("Found non-integer number, wrong type!");
        } else {
            if(item.value().get<int>() % 2 != 0){
                keys_to_remove.push_back(item.key());
            }
        }
    }

    for(const auto& key : keys_to_remove){
        json_object.erase(key);
    }
}

int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[argc - 1]));

        std::ifstream file(argv[argc - 1]);

        json data =  json::parse(file);

        ModifyJsonObject(data);

        for(const auto& item: data.items()){
            std::ofstream output_file(item.key() + ".json");
            output_file << item.value().dump(4);
            output_file.close();
        }
    }catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

