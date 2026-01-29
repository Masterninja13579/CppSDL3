add_executable(sandbox
    src/main.cpp
)

add_subdirectory(src/libraries/behaviorStates)
add_subdirectory(src/libraries/imgui-bgfx)

include_directories(
    ./src
)

target_link_libraries(sandbox PUBLIC
    SDL3::SDL3
    bgfx
    bx
    imgui
)

install(TARGETS sandbox RUNTIME DESTINATION bin)
