#pragma once

#include "platform/platform.h"

#include <imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bgfxImguiCompatibility/imgui.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

//#include <imgui_impl_sdl3.h>

#ifdef OS_WINDOWS 
    #include <windows.h>
    //#include <imgui_impl_dx12.h>
    #define PLATFORM_SDL_RENDER_FLAG 0
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Direct3D12;
#endif

#ifdef OS_MAC
    //#include <imgui_impl_metal.h>
    #define PLATFORM_SDL_RENDER_FLAG SDL_WINDOW_METAL
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Metal;
#endif

#ifdef OS_LINUX 
    //#include <imgui_impl_vulkan.h>
    #include "platform/linux/platformdata.h"
    #define PLATFORM_SDL_RENDER_FLAG SDL_WINDOW_VULKAN
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Vulkan;
#endif
