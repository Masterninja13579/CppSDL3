#pragma once

#include "core.h"

#include <string>

struct Shader
{
    std::string name = "";
    std::string sourcePath = "";
    std::string type = "";

    inline Shader() {}
    inline Shader(
        const std::string& name,
        const std::string& sourcePath,
        const std::string& type)
        : name(name), sourcePath(sourcePath), type(type)
    {
        
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shader,
    name,
    sourcePath,
    type
)
