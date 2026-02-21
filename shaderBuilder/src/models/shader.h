#pragma once

#include "core.h"

#include <string>

struct Shader
{
    std::string name = "";
    std::string filename = "";
    std::string sourcePath = "";
    std::string type = "";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Shader,
    name,
    filename,
    sourcePath
)
