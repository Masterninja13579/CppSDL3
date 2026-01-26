add_executable(sandbox
    src/main.cpp
 )

include_directories(
    ./src
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(sandbox PUBLIC
    SDL3::SDL3
    bgfx
    bx
    imgui
)

install(TARGETS sandbox RUNTIME DESTINATION bin)
