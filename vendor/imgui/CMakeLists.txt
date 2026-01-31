include(FetchContent)

set(IMGUI_VERSION 1.92.4)
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}.tar.gz
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

target_sources(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
)

target_link_libraries(
    imgui 
    PUBLIC 
    SDL3::SDL3
)
