#pragma once

#include "core.h"

#include "shader.h"

#include <string>

struct Project
{
    std::string name = "";
    bool dirty = false;

    std::vector<Shader> shaders;
    std::vector<char> shaderTab;

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
    dirty,
    shaders,
    shaderTab
)
