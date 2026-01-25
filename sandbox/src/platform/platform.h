#include "vendor.h"

#if defined(WIN32) || defined(_WIN32) || defined(__Win32__) || defined(__NT__)
    #define OS_WINDOWS 1
#elif __APPLE__
    #define OS_MAC 1
#elif __linux__
    #define OS_LINUX 1
#else
    #error "Unsupported Operating System"
#endif

#ifdef OS_WINDOWS 
    #include <windows.h>
    #define PLATFORM_SDL_RENDER_FLAG SDL_WINDOW_VULKAN
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Vulkan;
#endif

#ifdef OS_MAC 
    #define PLATFORM_SDL_RENDER_FLAG SDL_WINDOW_METAL
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Metal;
#endif

#ifdef OS_LINUX 
    #include "platform/linux/platformdata.h"
    #define PLATFORM_SDL_RENDER_FLAG SDL_WINDOW_VULKAN
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Vulkan;
#endif
