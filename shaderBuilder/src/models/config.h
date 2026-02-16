#pragma once

#include "core.h"

struct Config
{
    float cameraRotationRate = 0.2f;
    float cameraZoomMultiplier = 0.1f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, 
    cameraRotationRate, 
    cameraZoomMultiplier
)
