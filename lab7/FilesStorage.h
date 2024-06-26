#pragma once
#include <iostream>
#include <filesystem>
#include <set>
#include <string>

namespace fs = std::filesystem;

class FilesStorage
{
private:
    fs::path path_to_directory_;
    std::set<std::string> files_content_storage_;
public:
    void InitStorage();
    FilesStorage(const fs::path&  path_to_directory): path_to_directory_{path_to_directory}{}
    void CopyFile(const fs::path& path_to_file);
};



