include(FetchContent)

set(SDL3_VERSION 3.4.0)
FetchContent_Declare(
    sdl3
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-${SDL3_VERSION}.zip
)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

FetchContent_MakeAvailable(sdl3)

