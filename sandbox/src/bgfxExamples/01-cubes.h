#pragma once

#include "core.h"
#include "window/window.h"

#include "vs_cubes_dx11.h"
#include "vs_cubes_spirv.h"
#include "fs_cubes_dx11.h"
#include "fs_cubes_spirv.h"

#include <iostream>

namespace
{
    struct PosColorVertex
    {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_abgr;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosColorVertex::ms_layout;

    static PosColorVertex s_cubeVertices[] =
    {
        {-1.0f,  1.0f,  1.0f, 0xff000000 },
        { 1.0f,  1.0f,  1.0f, 0xff0000ff },
        {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
        { 1.0f, -1.0f,  1.0f, 0xff00ffff },
        {-1.0f,  1.0f, -1.0f, 0xffff0000 },
        { 1.0f,  1.0f, -1.0f, 0xffff00ff },
        {-1.0f, -1.0f, -1.0f, 0xffffff00 },
        { 1.0f, -1.0f, -1.0f, 0xffffffff },
    };

    static const uint16_t s_cubeTriList[] =
    {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    static const uint16_t s_cubeTriStrip[] =
    {
        0, 1, 2,
        3,
        7,
        1,
        5,
        0,
        4,
        2,
        6,
        7,
        4,
        5,
    };

    static const uint16_t s_cubeLineList[] =
    {
        0, 1,
        0, 2,
        0, 4,
        1, 3,
        1, 5,
        2, 3,
        2, 6,
        3, 7,
        4, 5,
        4, 6,
        5, 7,
        6, 7,
    };

    static const uint16_t s_cubeLineStrip[] =
    {
        0, 2, 3, 1, 5, 7, 6, 4,
        0, 2, 6, 4, 5, 7, 3, 1,
        0,
    };

    static const uint16_t s_cubePoints[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7
    };

    static const char* s_ptNames[]
    {
        "Triangle List",
        "Triangle Strip",
        "Lines",
        "Line Strip",
        "Points"
    };

    static const uint64_t s_ptState[]
    {
        UINT64_C(0),
        BGFX_STATE_PT_TRISTRIP,
        BGFX_STATE_PT_LINES,
        BGFX_STATE_PT_LINESTRIP,
        BGFX_STATE_PT_POINTS,
    };
    static_assert(BX_COUNTOF(s_ptState) == BX_COUNTOF(s_ptNames));

    
}

int bgfxCubes()
{
    //Create window
    Application::Window window("Cubes");
    window.Create();

    std::cout << "Rendering with " << bgfx::getRendererName(bgfx::getRendererType()) << "\n";
    
    // Create vertex stream declaration.
    PosColorVertex::init();

    // Create static vertex buffer.
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
        PosColorVertex::ms_layout
    );

    bgfx::IndexBufferHandle ibh[BX_COUNTOF(s_ptState)];
    // Create static index buffer for triangle list rendering.
	ibh[0] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );
	// Create static index buffer for triangle strip rendering.
	ibh[1] = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubeTriStrip, sizeof(s_cubeTriStrip))
	);
    // Create static index buffer for line list rendering.
	ibh[2] = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubeLineList, sizeof(s_cubeLineList))
	);
    // Create static index buffer for line strip rendering.
	ibh[3] = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubeLineStrip, sizeof(s_cubeLineStrip))
	);
    // Create static index buffer for point list rendering.
	ibh[4] = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(s_cubePoints, sizeof(s_cubePoints))
	);

#ifdef OS_WINDOWS
    const bgfx::Memory* vShaderData = bgfx::alloc(vs_cubes_dx11_size + 1);
    std::memcpy(vShaderData->data, vs_cubes_dx11, vs_cubes_dx11_size);
    vShaderData->data[vShaderData->size - 1] = '\0';

    const bgfx::Memory* fShaderData = bgfx::alloc(fs_cubes_dx11_size + 1);
    std::memcpy(fShaderData->data, fs_cubes_dx11, fs_cubes_dx11_size);
    fShaderData->data[fShaderData->size - 1] = '\0';
#else OS_LINUX
    const bgfx::Memory* vShaderData = bgfx::alloc(fs_cubes_spirv_size + 1);
    std::memcpy(vShaderData->data, vs_cubes_spirv, fs_cubes_spirv_size);
    vShaderData->data[vShaderData->size - 1] = '\0';

    const bgfx::Memory* fShaderData = bgfx::alloc(fs_cubes_spirv_size + 1);
    std::memcpy(fShaderData->data, fs_cubes_spirv, fs_cubes_spirv_size);
    fShaderData->data[fShaderData->size - 1] = '\0';
#endif
    bgfx::ShaderHandle vertexShaderHandle = bgfx::createShader(vShaderData);
    //bgfx::ShaderHandle fragmentShaderHandle = BGFX_INVALID_HANDLE;
    bgfx::ShaderHandle fragmentShaderHandle = bgfx::createShader(fShaderData);
    bgfx::ProgramHandle shaderProgramHandle = bgfx::createProgram(
        vertexShaderHandle,
        fragmentShaderHandle,
        true
    );

    //Create loop
    int counter = 0;
    bool showDebugStats = false;
    bool doStuff = true;
    while (doStuff)
    {
        counter++;
        //std::cout << counter << "\n";

        // Sleep if window is not visible
        if (window.GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

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


        // Set view and projection matrix for view 0.
        //{
        //    float view[16];
        //    bx::mtxLookAt(view, eye, at);

        //    float proj[16];
        //    bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        //    bgfx::setViewTransform(0, view, proj);

        //    // Set view 0 default viewport.
        //    bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
        //}


    }

    return EXIT_SUCCESS;
}
