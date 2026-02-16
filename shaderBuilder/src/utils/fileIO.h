#pragma once

#include "core.h"

#include <fstream>
#include <string>

template<typename T>
bool readJsonFile(const std::string& path, T& destination)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    try
    {
        json j;
        file >> j;
        destination = j.get<T>();
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
}

template<typename T>
bool writeJsonFile(const std::string& path, T& data, bool overwrite = false)
{
    if (std::filesystem::exists(path) && !overwrite)
        return false;
    
    std::ofstream file(path);
    if (!file.is_open())
        return false;
    
    try
    {
        json j = data;
        file << std::setw(4) << j;
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
}
