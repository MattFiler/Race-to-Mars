## set the project name to help import libs
project(GameClient)
set(GAMEDATA_FOLDER "data/client")

## add the files to be compiled here
set(SOURCE_FILES
        "client/main.cpp"
        "client/game.cpp"
        "client/FileHandler/FileHandler.cpp"
        "client/Debug/DebugText.cpp")

set(HEADER_FILES
        "client/game.h"
        "client/FileHandler/FileHandler.h"
        "client/Debug/DebugText.h" ../src/client/Cards/Card.h ../src/client/Cards/Card.cpp ../src/client/Cards/ObjectiveCard.cpp ../src/client/Cards/ObjectiveCard.h ../src/client/Cards/ItemCard.cpp ../src/client/Cards/ItemCard.h ../src/client/Cards/IssueCard.cpp ../src/client/Cards/IssueCard.h ../src/client/Core/ServiceLocator.h ../src/client/Players/Player.cpp ../src/client/Players/Player.h ../src/client/Players/MedicPlayer.cpp ../src/client/Players/MedicPlayer.h ../src/client/Players/EngineerPlayer.cpp ../src/client/Players/EngineerPlayer.h ../src/client/Players/CommunicationsPlayer.cpp ../src/client/Players/CommunicationsPlayer.h ../src/client/Players/PilotPlayer.cpp ../src/client/Players/PilotPlayer.h ../src/client/UI/Counter.h ../src/client/UI/Counter.cpp ../src/client/UI/PlayerCounter.cpp ../src/client/UI/PlayerCounter.h ../src/client/Core/ServiceLocator.cpp ../src/client/Math/Vector2.h ../src/client/Math/Vector3.h)

## the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

## the client is dependent on the game and network library
add_dependencies(${PROJECT_NAME} GameLib enetpp)
target_link_libraries(${PROJECT_NAME} GameLib enetpp)

## these are the build directories
get_target_property(CLIENT ${PROJECT_NAME} NAME)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/${CLIENT}/bin")

## important build scripts
include(build/compilation)
include(libs/asge)
include(libs/soloud)
include(tools/itch.io)

## hide console unless debug build ##
if (NOT CMAKE_BUILD_TYPE STREQUAL  "Debug" AND WIN32)
    target_compile_options(${PROJECT_NAME} -mwindows)
endif()