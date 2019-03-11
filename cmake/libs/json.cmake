## To enable JSON support override the default option setting
## set(ENABLE_JSON  ON  CACHE BOOL "Adds JSON Support" FORCE)

OPTION(ENABLE_JSON "Adds JSON to the Project" OFF)

if( ENABLE_JSON )

    # fetch project
    include(FetchContent)
    FetchContent_Declare(
            json
            URL https://github.com/nlohmann/json/releases/download/v3.5.0/include.zip)

    FetchContent_GetProperties(json)
    if(NOT json_POPULATED)
        FetchContent_Populate(json)

        # create a header only library
        add_library(jsonlib INTERFACE)
        target_include_directories(
                jsonlib
                INTERFACE SYSTEM
                ${json_SOURCE_DIR})

        # lets not worry about compile warnings
        target_compile_options(
                jsonlib
                INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-w -fpermissive>)
    endif()

    # link the project calling this script
    add_dependencies(${PROJECT_NAME} jsonlib)
    target_link_libraries(${PROJECT_NAME} jsonlib)

endif()
