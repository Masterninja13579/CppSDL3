#pragma once

#include "core.h"

#include <string>

struct Shader
{
    std::string name = "";
    std::string type = "";

    inline Shader() {}
    inline Shader(
        const std::string& name,
        const std::string& type)
        : name(name), type(type)
    {
        
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shader,
    name,
    type
)
