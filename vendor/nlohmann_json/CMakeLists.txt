include(FetchContent)

set(JSON_VERSION 3.12.0)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v${JSON_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${JSON_VERSION}
)
FetchContent_MakeAvailable(nlohmann_json)
