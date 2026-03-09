include(FetchContent)

#set(BUILD_SHARED_LIBS Off)
set(ASSIMP_VERSION 6.0.4)
FetchContent_Declare(
    assimp
    URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.4.zip
)
FetchContent_MakeAvailable(assimp)