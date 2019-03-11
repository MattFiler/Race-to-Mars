## include files
target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/src")

## fix issue with arch enabling gcc pie by default
## https://stackoverflow.com/questions/43367427/32-bit-absolute-addresses-no-longer-allowed-in-x86-64-linux
if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(${PROJECT_NAME} -no-pie pthread)
endif()

## enable static linking against gcc build
if (WIN32 AND NOT BUILD_SHARED_LIBS AND NOT MSVC)
    target_link_libraries(${PROJECT_NAME} -static)
endif()

target_compile_options(
        ${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${BUILD_FLAGS_FOR_CXX}>)
