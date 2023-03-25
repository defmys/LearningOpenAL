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

add_executable(${target} ${source})
target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
target_link_libraries(${target} ${openal})
