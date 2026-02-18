#pragma once

#include "core.h"

#include <string>

struct Project
{
    std::string name;

    inline Project(const std::string& name)
        : name(name)
    {
        
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Project, 
    name
)
