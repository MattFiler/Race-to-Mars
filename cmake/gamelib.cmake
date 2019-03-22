project(GameLib)

## shared library for the game
add_library(GameLib STATIC gamelib/gamelib.cpp
            ../src/gamelib/Constants.h
            ../src/gamelib/NetworkedData/Cards.h
            ../src/gamelib/NetworkedData/Players.h
            ../src/gamelib/NetworkedData/Rooms.h
        ../src/gamelib/Constants.h
        ../src/gamelib/NetworkedData/Cards.h
        ../src/gamelib/NetworkedData/Players.h
        ../src/gamelib/NetworkedData/Rooms.h
        ../src/gamelib/Constants.cpp
        ../src/gamelib/Debug/DebugText.cpp
        ../src/gamelib/Debug/DebugText.h
        ../src/gamelib/FileHandler/FileHandler.h
        ../src/gamelib/FileHandler/FileHandler.cpp
        ../src/gamelib/Localisation/Localisation.cpp
        ../src/gamelib/Localisation/Localisation.h
        ../src/gamelib/Math/Vector2.h
        ../src/gamelib/Math/BoundingBox.h
        ../src/gamelib/Packet.cpp
        ../src/gamelib/Packet.h)

## compile language settings
target_compile_features(GameLib PUBLIC cxx_std_17)

## add enet to gamelib
add_dependencies(${PROJECT_NAME} enetpp)
target_link_libraries(${PROJECT_NAME} enetpp)

## these are the build directories
get_target_property(GAMELIB GameLib NAME)
set_target_properties(GameLib
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${GAMELIB}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${GAMELIB}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${GAMELIB}/bin")

## include directories for this library
target_include_directories(GameLib
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/external/nlohmann")

include(build/compilation)
include(build/flags)
include(libs/asge)
include(libs/soloud)