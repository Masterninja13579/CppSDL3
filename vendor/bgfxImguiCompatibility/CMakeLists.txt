
project(bgfxImguiCompatibility VERSION 1.0.0 DESCRIPTION "Makes bgfx and imgui compatible.")

add_library(bgfxImguiCompatibility)

target_include_directories(
    bgfxImguiCompatibility
    PUBLIC
    ${bgfxImguiCompatibility_SOURCE_DIR}
)

target_sources(
    bgfxImguiCompatibility
    PUBLIC
    ${bgfxImguiCompatibility_SOURCE_DIR}/imgui.cpp
)
target_link_libraries(bgfxImguiCompatibility PUBLIC imgui)
