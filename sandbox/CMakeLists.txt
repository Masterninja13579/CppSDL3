add_executable(sandbox
    src/main.cpp
)

add_subdirectory(src/libraries)

include_directories(
    ./src
)

target_link_libraries(sandbox PUBLIC
    SDL3::SDL3
    bgfx
    bx
    bimg
    imgui
    subprocess
)

install(TARGETS sandbox RUNTIME DESTINATION bin)
