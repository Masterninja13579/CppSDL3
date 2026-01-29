include(FetchContent)

set(IMGUI_VERSION 1.92.4)
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${IMGUI_VERSION}
)
FetchContent_MakeAvailable(imgui)

# IMGUI doesn't support cmake, so we have to give cmake what it needs
add_library(imgui)

target_include_directories(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if (WIN32)
    set(PlatformSources ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
    set(PlatformLibraries d3dcompiler dwmapi)
elseif (APPLE)
    set(PlatformSources ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
    set(PlatformLibraries )
elseif (LINUX)
    set(PlatformSources ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
    set(PlatformLibraries Vulkan::Vulkan)
endif ()

target_sources(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${PlatformSources}
)

target_link_libraries(
    imgui 
    PUBLIC 
    SDL3::SDL3
    ${PlatformLibraries}
)
