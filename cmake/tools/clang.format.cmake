OPTION(ENABLE_CLANG_FORMAT "Adds clang-format style as a target" OFF)

find_program(
        CFE NAMES clang-format
        HINTS ${CMAKE_SOURCE_DIR}/tools/*/${PLATFORM}/)

file(GLOB_RECURSE SOURCE_CODE
        LIST_DIRECTORIES FALSE
        CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

if(CFE)
    list(APPEND CLANG-FORMAT_ARGS
            -i
            --style=file
            ${SOURCE_CODE}
    )

    add_custom_target(
            FormatYaCode
            COMMAND "${CFE}" ${CLANG-FORMAT_ARGS}
            COMMENT "running clang-format"
    )
endif()
