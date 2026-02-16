#pragma once

#include <window/window.h>

#include "models/config.h"
#include "models/session.h"
#include "objects/editorCamera.h"
#include "objects/editorGrid.h"
#include "utils/fileIO.h"

#include <bx/timer.h>

struct AppData
{
    bool applicationRun = true;
    bool drawGui = false;

    bool debugDisableGui = false;

    Application::Window* window = nullptr;
    Config config;

    bx::Ticks tickStart = bx::getNow();
    bx::Ticks tickCurrent = tickStart;
    bx::Ticks tickLast = tickStart;
    float timeDelta = 0.0f;
    float timeDuration = 0.0f;

    bool mouseInWindow = false;
    float mousePositionX = -1.0f;
    float mousePositionY = -1.0f;
    float mouseXMotion = 0.0f;
    float mouseYMotion = 0.0f;
    bool mouseGrab = false;

    EditorCamera camera;
    EditorGrid* grid = nullptr;

    Session session;

    AppData() {}
};
