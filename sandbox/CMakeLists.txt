add_executable(sandbox
    src/main.cpp
)

include_directories(
    ./src
)

target_link_libraries(sandbox PUBLIC
    libraries
)

install(TARGETS sandbox RUNTIME DESTINATION bin)
