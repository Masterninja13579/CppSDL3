cmake_minimum_required(VERSION 3.22)

project(libraries)

add_library(libraries STATIC
    behaviorStates/behaviorStateMachine.cpp
    bgfxTools/bgfxTools.cpp
    imgui-bgfx/imgui_impl_bgfx.cpp
    threading/threading.cpp
    window/window.cpp
)

target_include_directories(libraries PUBLIC
    ./
)

target_link_libraries(libraries PUBLIC
    bgfx
    bimg
    imgui
    SDL3::SDL3-static
    subprocess
    assimp
)
