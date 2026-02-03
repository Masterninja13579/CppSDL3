#pragma once

#include "core.h"
#include "bgfxExamples/logo.h"
#include "window.h"

#include <iostream>


int windowTest()
{
    //Create window
    Application::Window window("Application Window Test");
    window.Create();

    std::cout << "Rendering with " << bgfx::getRendererName(bgfx::getRendererType()) << "\n";
    
    //Create loop
    int counter = 0;
    bool showDebugStats = false;
    bool doStuff = true;
    while (doStuff)
    {
        counter++;
        std::cout << counter << "\n";

        // Sleep if window is not visible
        if (window.GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        const bgfx::Stats* stats = bgfx::getStats();
        uint16_t x = std::max<uint16_t>(uint16_t(stats->textWidth / 2), 20) - 20;
        uint16_t y = std::max<uint16_t>(uint16_t(stats->textHeight / 2), 6) - 6;

        bgfx::touch(0);
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x70, " Press SPACE to toggle bgfx render stats");
        bgfx::dbgTextPrintf(0, 1, 0x4f, " Frames: %d", counter);
        bgfx::dbgTextImage(
            x,
            y,
            40, // width
            12, // height
            s_logo, // data
            160 // pixel pitch
        );

        ImGui_Implbgfx_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

        bgfx::frame();

        // Events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    doStuff = false;
                    window.Destroy();
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    doStuff = false;
                    window.Destroy();
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    window.Refresh();
                    break;
                }
                case SDL_EVENT_KEY_DOWN:
                {
                    // Don't process keyboard events if ImGui has captured the keyboard
                    if (ImGui::GetIO().WantCaptureKeyboard)
                        break;

                    if (event.key.key == SDLK_SPACE)
                    {
                        showDebugStats = !showDebugStats;
                        bgfx::setDebug(showDebugStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
                    }
                    break;
                }
                default: break;
            }
        }

    }

    return EXIT_SUCCESS;
}