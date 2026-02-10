#pragma once

#include "core.h"
#include "window/window.h"

#include "appData.h"

#include <bx/timer.h>

void handleEvents(AppData& data)
{
    data.mouseXMotion = 0.0f;
    data.mouseYMotion = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                data.applicationRun = false;
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                data.applicationRun = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                data.window->Refresh();
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            {
                // Don't process keyboard events if ImGui has captured the keyboard
                if (ImGui::GetIO().WantCaptureKeyboard)
                    break;

                break;
            }
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                data.mouseInWindow = true;
                break;
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                data.mouseInWindow = false;
                data.mousePositionX = -1.0f;
                data.mousePositionY = -1.0f;
                data.mouseXMotion = 0.0f;
                data.mouseYMotion = 0.0f;
                break;
            case SDL_EVENT_MOUSE_MOTION:
            {
                if (data.mouseInWindow)
                {
                    if (data.mousePositionX >= 0.0f)
                        data.mouseXMotion = event.motion.x - data.mousePositionX;
                    if (data.mousePositionY)
                        data.mouseYMotion = event.motion.y - data.mousePositionY;
                    data.mousePositionX = event.motion.x;
                    data.mousePositionY = event.motion.y;

                    if (data.mouseGrab)
                    {
                        data.camera.rotationXZ -= 0.01f * data.mouseXMotion;
                        data.camera.rotationY = bx::clamp(
                            data.camera.rotationY + 0.01f * data.mouseYMotion,
                            -bx::kPiHalf + 0.01f,
                            bx::kPiHalf - 0.01f);
                    }
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    data.mouseGrab = false;
                    std::cout << "Grab released\n";
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT && data.mouseInWindow)
                {
                    data.mouseGrab = true;
                    std::cout << "Grab pressed\n";
                }
                break;
            }
            default: break;
        }
    }
}

void renderObjects(AppData& data)
{
    const int width = data.window->GetWidth();
    const int height = data.window->GetHeight();

    {
        float view[16];
        data.camera.apply(view);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    }

    bgfx::touch(0);

    data.grid->render();

    bgfx::frame();
}

int shaderBuilder()
{
    //Create window
    Application::Window window("Cubes");
    window.Create();
    
    AppData data;
    data.window = &window;
    if (!EditorGrid::Initialize())
    {
        window.Destroy();
        return EXIT_SUCCESS;
    }
    data.grid = new EditorGrid(10, 10);

    //Create loop
    while (data.applicationRun)
    {
        // Set up time variables
        data.tickLast = data.tickCurrent;
        data.tickCurrent = bx::getNow();
        data.timeDelta = bx::toSeconds<float>(data.tickCurrent - data.tickLast);
        data.timeDuration = bx::toSeconds<float>(data.tickCurrent - data.tickStart);

        // Sleep if window is not visible
        if (window.GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        handleEvents(data);
        
        renderObjects(data);
    }

    delete data.grid;
    window.Destroy();

    return EXIT_SUCCESS;
}
