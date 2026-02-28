#pragma once

#include "core.h"

#include "defines.h"
#include "project.h"
#include "tab.h"

#include <vector>

struct Session
{
    std::string projectName = "";

    std::vector<Tab> openTabs;
    int selectedTab = -1;

    bool newShaderTabCreate = true;
    bool newShaderTabImport = false;
    int newShaderTypeSelection = 0;

    bool selectorShadersOpen = true;
    bool selectorLibrariesOpen = true;
    bool selectorOutputsOpen = true;
    int selectedObject = -1;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Session, 
    projectName,

    openTabs,
    selectedTab,

    newShaderTabCreate,
    newShaderTabImport,
    newShaderTypeSelection,

    selectorShadersOpen,
    selectorLibrariesOpen,
    selectorOutputsOpen,
    selectedObject
)
