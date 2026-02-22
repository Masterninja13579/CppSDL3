#pragma once

#include "core.h"

#include "defines.h"
#include "project.h"

#include <vector>

struct Session
{
    std::string projectName = "";
    std::vector<char> shaderTab;

    bool newShaderTabCreate = true;
    bool newShaderTabImport = false;
    int newShaderTypeSelection = 0;

    bool selectorShadersOpen = true;
    bool selectorLibrariesOpen = true;
    bool selectorOutputsOpen = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Session, 
    projectName,
    shaderTab,

    newShaderTabCreate,
    newShaderTabImport,
    newShaderTypeSelection,

    selectorShadersOpen,
    selectorLibrariesOpen,
    selectorOutputsOpen
)
