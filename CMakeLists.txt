cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(CppSDL3 VERSION 0.1.0 LANGUAGES C CXX)

# Library folders
add_subdirectory(vendor)
add_subdirectory(libraries)

# projects
add_subdirectory(sandbox)
add_subdirectory(shaderBuilder)
