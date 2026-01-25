#pragma once

#include "core.h"

#include <iostream>

struct BgfxNullCallback : public bgfx::CallbackI
{
		virtual void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override {}
		virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override {}
		virtual void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {}
		virtual void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {}
		virtual void profilerEnd() override {}
		virtual uint32_t cacheReadSize(uint64_t _id) override { return 0; }
		virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override { return false; }
		virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override {}
		virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override {}
		virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override {}
		virtual void captureEnd() override {}
		virtual void captureFrame(const void* _data, uint32_t _size) override {}
};

int bgfxTest()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "ERROR: failed to initialize SDL\n";
        return EXIT_FAILURE;
    }

    const int width = 1080;
    const int height = 720;
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
    if (!setLinuxPlatformData(sdlPropertiesId, pd))
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
    init.callback = new BgfxNullCallback();
    if (!bgfx::init(init))
    {
        std::cout << "ERROR: failed to initialize bgfx\n";
        return EXIT_FAILURE;
    }

    bgfx::setDebug(BGFX_DEBUG_NONE);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);

    bgfx::shutdown();
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    std::cout << "Success!\n";
    return EXIT_SUCCESS;
}
