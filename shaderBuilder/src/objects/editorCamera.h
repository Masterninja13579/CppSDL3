#pragma once

#include "core.h"

#include <bx/math.h>

struct EditorCamera
{
    bx::Vec3 origin = { 0.0f, 0.0f, 0.0f };
    float rotationXZ = 0.0f;
    float rotationY = 0.785f;
    float distance = 10.0f;

    EditorCamera();

    void apply(float* viewMatrix);
};
