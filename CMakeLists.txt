cmake_minimum_required(VERSION 3.19.0)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++20 \
    -Wall \
    -Werror \
    -I/usr/local/include \
    -I/opt/homebrew/include \
    ")

set(project_root ${CMAKE_CURRENT_SOURCE_DIR})
set(source_root ${project_root}/src)

project( learning_open_al )

find_library(
    openal
    openal
    REQUIRED 
    PATHS /opt/homebrew/opt/openal-soft/lib
)

find_package(SndFile REQUIRED)

add_subdirectory(src/Framework)

add_custom_target(
    copy_resources
    COMMAND cp -r ${CMAKE_CURRENT_LIST_DIR}/resource/ ${CMAKE_CURRENT_BINARY_DIR}/resource/
)
