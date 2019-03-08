## set the project name to help import libs
project(GameServer)
set(GAMEDATA_FOLDER "data/server")

## add the files to be compiled here
set(SOURCE_FILES
        "server/main.cpp")

set(HEADER_FILES
        "")

## the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## the server is dependent on the game and network library
add_dependencies(${PROJECT_NAME} GameLib enetpp)
target_link_libraries(${PROJECT_NAME} GameLib enetpp)

## these are the build directories
get_target_property(SERVER GameServer NAME)
set_target_properties(GameServer
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${SERVER}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${SERVER}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${SERVER}/bin")

## important build scripts
include(build/compilation)
include(tools/itch.io)