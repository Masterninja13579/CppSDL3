#pragma once

#include "core.h"

#include "project.h"

#include <vector>

struct Session
{
    Project project;

    bool newShaderTabCreate = true;
    bool newShaderTabImport = false;
    int newShaderTypeSelection = 0;

    inline bool isProjectOpen() { return project.name.size() > 0; }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Session, 
    project
)
