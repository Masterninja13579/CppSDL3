#pragma once

#include "platform/platform.h"

#include <nlohmann/json.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui-bgfx/imgui_impl_bgfx.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/uint32_t.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

#ifdef OS_WINDOWS 
    #define PLATFORM_SDL_RENDER_FLAG 0
    #define PLATFORM_BGFX_RENDERERTYPE bgfx::RendererType::Direct3D11;
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

using json = nlohmann::json;
