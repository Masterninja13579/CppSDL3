#pragma once

#include "core.h"

#include <string>

struct Project
{
    std::string name;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Project, 
    name
)
