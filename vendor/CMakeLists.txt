if (LINUX)
    find_package(Vulkan REQUIRED)
endif()

add_subdirectory(imgui)
add_subdirectory(sdl3)
add_subdirectory(bgfx)

