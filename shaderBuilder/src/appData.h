#pragma once

#include <window/window.h>

#include "defines.h"
#include "models/config.h"
#include "models/session.h"
#include "objects/editorCamera.h"
#include "objects/editorGrid.h"
#include "utils/fileIO.h"

#include <bx/timer.h>

struct AppData
{
    bool applicationRun = true;
    
    Application::Window* window = nullptr;
    
    Config config;
    Session session;
    Project project;
    
    bool isProjectDirty = false;
    bool doNewProjectPopup = false;
    bool doNewShaderPopup = false;

    char newProjectName[PROJECT_NAME_SIZE] = PROJECT_NAME_DEFAULT;
    char newShaderName[SHADER_NAME_SIZE] = SHADER_NAME_DEFAULT;

    AppData() {}
};
