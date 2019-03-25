## set the project name to help import libs
project(GameClient)
set(GAMEDATA_FOLDER "data/client")

## add the files to be compiled here
set(SOURCE_FILES
        "client/main.cpp"
        "client/game.cpp"
        "client/game.h"

        ../src/client/Cards/Card.cpp
        ../src/client/Cards/ObjectiveCard.cpp
        ../src/client/Cards/ItemCard.cpp
        ../src/client/Cards/IssueCard.cpp
        ../src/client/Players/Player.cpp
        ../src/client/Players/MedicPlayer.cpp
        ../src/client/Players/EngineerPlayer.cpp
        ../src/client/Players/CommunicationsPlayer.cpp
        ../src/client/Players/PilotPlayer.cpp
        ../src/client/UI/OnBoardObject.cpp
        ../src/client/Board/PlayerCounter.cpp
        ../src/client/Core/ServiceLocator.cpp
        ../src/client/Scenes/SceneManager.cpp
        ../src/client/Scenes/GameScene.cpp
        ../src/client/Scenes/MenuScene.cpp
        ../src/client/KeyHandler/KeyHandler.cpp
        ../src/client/NetworkConnection/NetworkConnection.cpp
        ../src/client/Cards/Deck.cpp
        ../src/client/Board/Ship.cpp
        ../src/client/UI/Menu.cpp
        ../src/client/Board/GameBoard.cpp
        ../src/client/Board/ShipRoom.cpp

        )

set(HEADER_FILES
        ../src/client/Cards/Card.h
        ../src/client/Cards/ObjectiveCard.h
        ../src/client/Cards/ItemCard.h
        ../src/client/Cards/IssueCard.h
        ../src/client/Core/ServiceLocator.h
        ../src/client/Players/Player.h
        ../src/client/Players/MedicPlayer.h
        ../src/client/Players/EngineerPlayer.h
        ../src/client/Players/CommunicationsPlayer.h
        ../src/client/Players/PilotPlayer.h
        ../src/client/UI/OnBoardObject.h
        ../src/client/Board/PlayerCounter.h
        ../src/client/Scenes/SceneManager.h
        ../src/client/Scenes/Scene.h
        ../src/client/Scenes/GameScene.h
        ../src/client/Scenes/MenuScene.h
        ../src/client/KeyHandler/KeyHandler.h
        ../src/client/Players/AllPlayers.h
        ../src/client/NetworkedData/PlayerData.h
        ../src/client/NetworkConnection/NetworkConnection.h
        ../src/client/Cards/Deck.h
        ../src/client/NetworkConnection/NetworkConnection.h
        ../src/client/Board/Ship.h
        ../src/client/UI/Menu.h
        ../src/client/Board/GameBoard.h
        ../src/client/Board/ShipRoom.h
        ../src/client/Board/ShipRoom.cpp ../src/client/UI/Cursor.h ../src/client/UI/Cursor.cpp ../src/client/UI/ScaledSprite.cpp ../src/client/UI/ScaledSprite.h ../src/gamelib/NetworkedData/MessageTypes.h ../src/gamelib/NetworkedData/NetworkedData.h ../src/client/Players/ClientPlayer.cpp ../src/client/Players/ClientPlayer.h ../src/client/Scenes/LobbyScene.cpp ../src/client/Scenes/LobbyScene.h ../src/server/Structs/Lobby.h ../src/server/Structs/ServerClient.h)

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

## Copy launcher to build directory on Windows
if (${PLATFORM} MATCHES "win")
    configure_file("${CMAKE_SOURCE_DIR}/launcher/RTS_Launcher.exe" "${CMAKE_BINARY_DIR}/build/${CLIENT}/bin/Launcher.exe" COPYONLY)
endif()

## important build scripts
include(build/compilation)
include(tools/itch.io)

## hide console unless debug build ##
if (NOT CMAKE_BUILD_TYPE STREQUAL  "Debug" AND WIN32)
    target_compile_options(${PROJECT_NAME} -mwindows)
endif()