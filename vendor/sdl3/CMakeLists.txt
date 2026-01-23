include(FetchContent)

set(SDL3_VERSION 3.4.0)
FetchContent_Declare(
    sdl3
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-${SDL3_VERSION}.zip
    FIND_PACKAGE_ARGS ${SDL3_VERSION}
)
FetchContent_MakeAvailable(sdl3)
