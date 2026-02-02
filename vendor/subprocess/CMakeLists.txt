include(FetchContent)

set(SUBPROCESS_VERSION 0.6.0)
FetchContent_Declare(
    subprocess
    URL https://github.com/benman64/subprocess/archive/refs/tags/v${SUBPROCESS_VERSION}.tar.gz
)
FetchContent_MakeAvailable(subprocess)
