#include "fileIO.h"

bool ensureDirectories(const std::string& dirpath)
{
    std::filesystem::path path(dirpath);
    return std::filesystem::create_directories(path);
}

bool readTextFile(const std::string& path, std::string& output)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return false;

    size_t size = (size_t)file.tellg();
    output.resize(size);
    
    file.seekg(0, std::ios::beg);
    file.read(output.data(), size);
    return true;
}
bool writeTextFile(const std::string& path, const std::string& content, bool overwrite)
{
    if (std::filesystem::exists(path) && !overwrite)
        return false;
    
    std::ofstream file(path);
    if (!file.is_open())
        return false;
    
    file << content;
    return true;
}

