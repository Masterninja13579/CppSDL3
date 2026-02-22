#pragma once

#include "core.h"

#include "shader.h"

#include <string>

struct Project
{
    std::string name = "";
    std::vector<Shader> shaders;

    inline Project()
    {

    }
    inline Project(const std::string& name)
        : name(name)
    {
        
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Project, 
    name,
    shaders
)
