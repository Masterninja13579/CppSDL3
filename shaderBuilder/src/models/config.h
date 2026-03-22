#pragma once

#include "core.h"

#include "defines.h"

struct Config
{
    int guiSelectionPanelWidth = GUI_SELECTIONPANEL_DEFAULT_SIZE;
    int guiOutputPanelHeight = GUI_OUTPUTPANEL_DEFAULT_SIZE;
    int guiVaryingPanelWidth = GUI_VARYINGPANEL_DEFAULT_SIZE;

    std::string shadersPath = SHADER_DEFAULT_PATH;
    std::string projectsPath = PROJECT_DEFAULT_PATH;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, 
    guiSelectionPanelWidth,
    guiOutputPanelHeight,
    guiVaryingPanelWidth,

    shadersPath,
    projectsPath
)
