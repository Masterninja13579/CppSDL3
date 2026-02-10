#pragma once

#include <window/window.h>

#include "config.h"
#include "objects/editorCamera.h"
#include "objects/editorGrid.h"

#include <bx/timer.h>

struct AppData
{
    bool applicationRun = true;

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

    AppData() {}
};
