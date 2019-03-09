cmake_minimum_required(VERSION 3.11.4)
project(MyNetGame)
set(ENABLE_ENET  ON  CACHE BOOL "Adds Networking"   FORCE)
set(ENABLE_SOUND ON  CACHE BOOL "Adds SoLoud Audio" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

## we need enet ##
include(libs/enetpp)

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/external/nlohmann")

## out of source builds ##
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
## out of source builds ##

## projects ##
include(gamelib)
include(server)
include(client)
## projects ##

