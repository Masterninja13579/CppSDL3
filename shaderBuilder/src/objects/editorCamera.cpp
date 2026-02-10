#include "editorCamera.h"

EditorCamera::EditorCamera()
{

}

void EditorCamera::apply(float* viewMatrix)
{
    float y = distance * bx::sin(rotationY);
    float xzDist = distance * bx::cos(rotationY);
    float x = xzDist * bx::cos(rotationXZ - bx::kPiHalf);
    float z = xzDist * bx::sin(rotationXZ - bx::kPiHalf);

    bx::Vec3 eye = bx::add({ x, y, z }, origin);
    bx::mtxLookAt(viewMatrix, eye, origin);
}
