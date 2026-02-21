add_executable(shaderBuilder
    src/main.cpp
    src/appFunctions.cpp

    src/objects/editorCamera.cpp
    src/objects/editorGrid.cpp

    src/gui/gui.cpp
    src/gui/mainMenuBar.cpp
    src/gui/popups.cpp
    src/gui/selectorPanel.cpp    
)

include_directories(
    ./src
)

target_link_libraries(shaderBuilder PUBLIC
    libraries
)

set_target_properties(shaderBuilder PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/shaderBuilder"
)

install(TARGETS shaderBuilder RUNTIME DESTINATION bin)
