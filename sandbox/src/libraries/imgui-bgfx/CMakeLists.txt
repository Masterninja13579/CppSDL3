
target_include_directories(
    sandbox
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(
    sandbox
    PUBLIC
    imgui_impl_bgfx.cpp
)
