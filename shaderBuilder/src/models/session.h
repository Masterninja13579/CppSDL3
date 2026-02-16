#pragma once

#include "core.h"

#include "project.h"

#include <vector>

struct Session
{
    std::vector<Project> openProjects;
    int selectedProject;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Session, 
    openProjects,
    selectedProject
)
