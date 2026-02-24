add_executable(sandbox
    src/main.cpp

    src/bgfxExamples/00-helloWorld.cpp
    src/bgfxExamples/01-cubes.cpp
    src/bgfxExamples/02-metaballs.cpp
    src/bgfxExamples/03-raymarch.cpp

    src/experiments/embedExampleShaders.cpp
    src/experiments/threadPoolTest.cpp
)

include_directories(
    ./src
)

target_link_libraries(sandbox PUBLIC
    libraries
)

set_target_properties(sandbox PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/sandbox"
)

install(TARGETS sandbox RUNTIME DESTINATION bin)
