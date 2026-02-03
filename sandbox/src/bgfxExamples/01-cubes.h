#pragma once

#include "core.h"

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
        "Points",
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
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "ERROR: failed to initialize SDL\n";
        return EXIT_FAILURE;
    }

    int width = 1080;
    int height = 720;
    SDL_Window* sdlWindow = SDL_CreateWindow(
        "SDL3 + bgfx + ImGui",
        width,
        height,
        SDL_WINDOW_RESIZABLE | PLATFORM_SDL_RENDER_FLAG);
    if (!sdlWindow)
    {
        std::cout << "ERROR: failed to create an SDL window\n";
        return EXIT_FAILURE;
    }

    SDL_PropertiesID sdlPropertiesId = SDL_GetWindowProperties(sdlWindow);
    bgfx::PlatformData pd{};
#ifdef OS_WINDOWS
    pd.nwh = SDL_GetPointerProperty(sdlPropertiesId, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    pd.ndt = NULL;
#elif OS_LINUX
    if (!Platform_SetLinuxPlatformData(sdlPropertiesId, pd))
    {
        std::cout << "ERROR: failed to identify linux platform data\n";
        return EXIT_FAILURE;
    }
#elif OS_MAC
    pd.nwh = SDL_GetPointerProperty(sdlPropertiesId, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    pd.ndt = NULL;
#endif
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);

    bgfx::Init init;
    init.type = PLATFORM_BGFX_RENDERERTYPE;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.platformData.nwh = pd.nwh;
    init.platformData.ndt = pd.ndt;
    init.resolution.width = width;
    init.resolution.height = height;
    //init.callback = new BgfxNullCallback();
    if (!bgfx::init(init))
    {
        std::cout << "ERROR: failed to initialize bgfx\n";
        return EXIT_FAILURE;
    }
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);

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

    bgfx::ProgramHandle shaderProgram;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_Implbgfx_Init(255); // not sure why it's using 255?  could remove the need for this.
#if OS_WINDOWS
    ImGui_ImplSDL3_InitForD3D(sdlWindow);
#elif OS_MAC
    ImGui_ImplSDL3_InitForMetal(sdlWindow);
#elif OS_LINUX
    ImGui_ImplSDL3_InitForVulkan(sdlWindow);
#endif

    std::cout << "Rendering with " << bgfx::getRendererName(bgfx::getRendererType()) << "\n";

    bool doStuff = true;
    while (doStuff)
    {
        // Events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    doStuff = false;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    if (event.window.windowID == SDL_GetWindowID(sdlWindow))
                        doStuff = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    SDL_GetWindowSize(sdlWindow, &width, &height);
                    bgfx::reset(width, height, BGFX_RESET_NONE);
                    bgfx::setViewRect(0, 0, 0, width, height);
                }
                default: break;
            }
        }

        // Sleep if window is not visible
        if (SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        const bgfx::Stats* stats = bgfx::getStats();
        uint16_t x = std::max<uint16_t>(uint16_t(stats->textWidth/2), 20) - 20;
        uint16_t y = std::max<uint16_t>(uint16_t(stats->textHeight/2), 6) - 6;
        
        bgfx::touch(0);
        bgfx::dbgTextClear();
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
    }

    ImGui_ImplSDL3_Shutdown();
    ImGui_Implbgfx_Shutdown();
    ImGui::DestroyContext();
    bgfx::shutdown();
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}
