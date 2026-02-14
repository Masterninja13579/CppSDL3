if (LINUX)
    find_package(Vulkan REQUIRED)
endif()

add_subdirectory(bgfx)
add_subdirectory(imgui)
add_subdirectory(sdl3)
add_subdirectory(subprocess)
add_subdirectory(nlohmann_json)

