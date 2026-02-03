#pragma once

bool Platform_SetLinuxPlatformData(SDL_PropertiesID& id, bgfx::PlatformData& pd)
{
    void* ptr = SDL_GetPointerProperty(id, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
    if (ptr)
    {
        pd.ndt = ptr;
        pd.nwh = SDL_GetPointerProperty(id, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        return true;
    }

    ptr = SDL_GetPointerProperty(id, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    if (ptr)
    {
        pd.ndt = ptr;
        pd.nwh = (void*) SDL_GetNumberProperty(id, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        return true;
    }

    return false;
}
