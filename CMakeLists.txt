cmake_minimum_required(VERSION 3.19.0)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++20 \
    -Wall \
    -Werror \
    -I/usr/local/include \
    -I/opt/homebrew/include \
    ")

project( learning_open_al )

set( target learning_open_al )
file(GLOB_RECURSE source src/*.cpp)

find_library(
    openal
    openal
    REQUIRED 
    PATHS /opt/homebrew/opt/openal-soft/lib
)

find_package(SndFile REQUIRED)

add_executable(${target} ${source})
add_dependencies(${target} copy_resources)
target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
target_link_libraries(${target} ${openal} SndFile::sndfile)

add_custom_target(
    copy_resources
    COMMAND cp -r ${CMAKE_CURRENT_LIST_DIR}/resource/ ${CMAKE_CURRENT_BINARY_DIR}/resource/
)
