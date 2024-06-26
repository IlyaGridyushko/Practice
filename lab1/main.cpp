#include <iostream>
#include "json.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>

//variant 4
/*
    Given task: 
    Create a C++ program that reads a JSON and check its structure.
    {
    "string_0" : "some string",
    "number" : 10,
    "object" :
        {
        "boolean" : true,
        "array" : []
        },
    "string_1" : null,
    "array" : null
    }
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

bool check_structure(const json& data, const json& template_json) {
    if (data.type() != template_json.type()) {
        return false;
    }

    if (data.is_object()) {
        for (const auto& item : template_json.items()) {
            auto key = item.key();
            if (!data.contains(key)) {
                return false;
            }
            if (!check_structure(data[key], template_json[key])) {
                return false;
            }
        }
        for (const auto& item : data.items()) {
            auto key = item.key();
            if (!template_json.contains(key)) {
                return false;
            }
        }
    } else if (data.is_array()) {
        if (!template_json.empty() && !data.empty()) {
            if(data.size()!= template_json.size()){
                return false;
            }
            for(int i=0; i<data.size(); i++){
                if(!check_structure(data[i], template_json[i])){
                    return false;
                }
            }
        }
    }

    return true;
}
int main(int argc, char *argv[]){
    try{
        CheckArgumentsAmount(argc);
        CheckInputPath(fs::path(argv[1]));
        json template_json = R"({
            "string_0" : "some string",
            "number" : 10,
            "object" : {
                "boolean" : true,
                "array" : []
            },
            "string_1" : null,
            "array" : null
            })"_json;
        std::ifstream file(argv[argc - 1]);
        json data =  json::parse(file);

        if(check_structure(data, template_json)){
            std::cout << "Structure is correct" << std::endl;
        }else{
            std::cout << "Structure is not correct" << std::endl;
        }
        
    }catch(const std::exception& e){
        std::cerr<< "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

