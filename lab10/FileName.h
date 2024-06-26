#if __cplusplus < 202002L
#error "C++20 or higher is required"
#endif
#pragma once
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

class FileName{
    private:
       
    public:
        fs::path path_to_current_file;
        std::uint16_t year;
        std::uint16_t month;
        std::uint16_t day;
        std::string only_name;
        void Parse();
        FileName(fs::path path_to_file):path_to_current_file(path_to_file){}
        bool IsRemoveRequired() const;
};
