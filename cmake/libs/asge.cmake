## build the ASGE game libs folder

if (TARGET ASGE)
  return()
endif()

add_library(ASGE UNKNOWN IMPORTED)
target_link_libraries(${PROJECT_NAME} ASGE)

## ASGE is out of the build tree, specify binary dir
add_subdirectory(
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_BINARY_DIR}/_deps/asge)

## library includes
target_include_directories(
        ${PROJECT_NAME}
        SYSTEM
        PUBLIC
        "${CMAKE_SOURCE_DIR}/external/asge/include")

## copy the dlls post build
if(BUILD_SHARED_LIBS)
    # copy the game engine dll to bin
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different

            "${GameEngineDLL}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    if(WIN32 AND NOT MSVC)
        # copy the mingw runtime files to bin
        add_custom_command(
                TARGET ${PROJECT_NAME} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/external/mingw64/bin $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()
endif()
