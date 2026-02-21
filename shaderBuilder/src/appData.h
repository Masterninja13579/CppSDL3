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
    
    // bx::Ticks tickStart = bx::getNow();
    // bx::Ticks tickCurrent = tickStart;
    // bx::Ticks tickLast = tickStart;
    // float timeDelta = 0.0f;
    // float timeDuration = 0.0f;
    
    // bool mouseInWindow = false;
    // float mousePositionX = -1.0f;
    // float mousePositionY = -1.0f;
    // float mouseXMotion = 0.0f;
    // float mouseYMotion = 0.0f;
    // bool mouseGrab = false;
    
    // EditorCamera camera;
    // EditorGrid* grid = nullptr;
    
    Config config;
    Session session;

    bool doNewProjectPopup = false;
    bool doNewShaderPopup = false;

    char newProjectName[PROJECT_NAME_SIZE] = PROJECT_NAME_DEFAULT;
    char newShaderName[SHADER_NAME_SIZE] = SHADER_NAME_DEFAULT;

    AppData() {}
};
